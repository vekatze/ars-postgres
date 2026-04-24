# ars-postgres

`ars-postgres` is a PostgreSQL client library for the [Neut](https://vekatze.github.io/neut/) programming language.

## Installation

### Install Prerequisites

Install `libpq` and `pkg-config`:

```sh
# Debian, Ubuntu
apt install libpq-dev pkg-config

# macOS
brew install libpq pkg-config
```

On macOS, set:

```sh
export PKG_CONFIG_PATH=/opt/homebrew/opt/libpq/lib/pkgconfig:$PKG_CONFIG_PATH
```

Ensure that the following command runs successfully:

```sh
pkg-config libpq --libs --cflags
```

### Add this Module to Your Project

Add `ars-postgres` to your project:

```sh
neut get ars-postgres https://github.com/vekatze/ars-postgres/raw/main/archive/0-1-54.tar.zst
```

### Configure Your Project

Finally, edit your project's `module.ens`:

```ens
{
  // ...
  target {
    your-project {
      main "your-project.nt",
      // ↓ add
      build-option [
        "$(pkg-config libpq --libs --cflags)",
      ],
      // ↑ add
    },
  },
  zen {
    // ↓ add
    build-option [
      "$(pkg-config libpq --libs --cflags)",
    ],
    // ↑ add
  },

  // ...
}
```

## Types

### Main Definitions

```neut
// Represents a connection info.
data conninfo {
| Conn-Info(
    host: string,
    port: string,
    dbname: string,
    user: string,
    password: string,
  )
}

// Represents an error happened while establishing a connection.
data connection-error {
| Connection-Error(message: string)
}

// Represents an open connection.
data connection

// Establishes a connection to a DB.
// A connection is closed when the corresponding variable is not used.
// A connection is copied when the corresponding variable is used more than once.
define connect(c: conninfo) -> either(connection-error, connection)

// Executes a parameterized SQL command.
inline execute<a>(conn: &connection, com: command(a)) -> either(error, a)

// Executes a raw SQL command.
define batch(c: &connection, com: &string) -> either(error, unit)

// Executes `begin`.
define begin(c: &connection) -> either(error, unit)

// Executes `commit`.
define commit(c: &connection) -> either(error, unit)

// Executes `rollback`.
define rollback(c: &connection) -> either(error, unit)

// Executes the given action in a `begin ... commit`.
// `rollback` is executed instead of `commit` if `action` results in an error.
define with-transaction<a>(c: &connection, action: () -> either(error, a)) -> either(error, a)
```

### Command

```neut
// Represents a parameterized SQL command and a way to encode the result of the command.
// See the `Command` section of `Example` in this readme.
data command(a) {
| Command(
    template: &string,
    parameters: list(pair(&string, parameter)),
    result-encoder: (&table) -> either(encoder-error, a),
  )
}

// Represents the value of a placeholder in a SQL command.
data parameter {
| Integer(int)
| Float(float)
| Text(string)
| Time(string)
| Timestamp(time)
| Tuple(list(parameter))
}

// Ignores the response table.
define void-encoder(_: &table) -> either(encoder-error, unit)

// Creates a command that just executes the given SQL string.
define statement(t: &string) -> command(unit)
```

### Table

```neut
// Represents a SQL table.
data table

// Gets the column size of a table.
define get-column-size(r: &table) -> int

// Gets the row size of a table.
define get-row-size(r: &table) -> int

// Gets the column number from a table by specifying column name.
define get-column-by-name(r: &table, column-name: &string) -> either(encoder-error, int)

// Reads the value at (row, column) in the given table as an integer.
inline encode-int(r: &table, row: int, column: int) -> either(encoder-error, int)

// Reads the value at (row, column) in the given table as a float.
inline encode-float(r: &table, row: int, column: int) -> either(encoder-error, float)

// Reads the value at (row, column) in the given table as a string.
inline encode-string(r: &table, row: int, column: int) -> either(encoder-error, string)

// Reads the value at (row, column) in the given table as an ISO8601 time.
inline encode-time(r: &table, row: int, column: int) -> either(encoder-error, time)
```

## Example

Suppose we created a table `author` as follows:

```sql
create table if not exists author (
  id bigint primary key,
  name text not null
)
```

### Basics

The following code demonstrates a basic usage of `ars-postgres`:

```neut
import {
  core.list {iterate-E},
  core.string.io {print-line},
  this.action {batch, execute, with-transaction},
  this.command {Command, command},
  this.connection {Conn-Info, Connection-Error, connect},
  this.parameter {Integer, Text},
  this.table {encode-string, get-column-by-name, get-row-size, table},
}

define get-conninfo() -> this.connection.conninfo {
  Conn-Info{
    host := *"127.0.0.1",
    port := *"5432",
    dbname := *"sample",
    user := *"username",
    password := *"password",
  }
}

define connect-then-insert-then-select() -> unit {
  // Establishes a DB connection
  let conn-or-none = connect(get-conninfo());
  match conn-or-none {
  | Left(Connection-Error(message)) =>
    pin message = message;
    print-line(message)
  | Right(conn) =>
    let result on conn =
      with-transaction(conn, () => {
        // Executes SQL commands using a connection
        try _ = batch(conn, "create table if not exists author (id bigint primary key, name text not null)");
        try _ =
          execute(conn, Command{
            template := "insert into author (id, name) values (__ID__, __NAME__)",
            parameters := List[
              Pair("__ID__", Integer(1)),
              Pair("__NAME__", Text(*"Virginia Woolf")),
            ],
            result-encoder := this.command.void-encoder,
          });
        execute(conn, select-authors(5))
      });
    // (conn is closed here because it is discarded here)
    match result {
    | Right(author-list) =>
      let _ = author-list;
      // use `author-list` as you like
      print("success\n")
    | Left(_) =>
      print("error\n")
    }
  }
}
```

### Defining Commands

Placeholders in `template` are arbitrary marker strings such as `__NAME__`. When executing a command, they are replaced with PostgreSQL parameters like `$1`, `$2`, and so on.

A command that performs `select` can be defined, for example, as follows:

```neut
define select-authors(limit: int) -> command(list(string)) {
  Command{
    template := "select name from author order by id limit __LIMIT__",
    parameters := List[Pair("__LIMIT__", Integer(limit))],
    result-encoder := {
      (res: &table) => {
        try name-column = get-column-by-name(res, "name");
        let row-size = get-row-size(res);
        iterate-E(row-size, (row) => {
          encode-string(res, row, name-column)
        })
      }
    },
  }
}
```
