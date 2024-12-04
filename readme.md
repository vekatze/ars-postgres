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
neut get ars-postgres https://github.com/vekatze/ars-postgres/raw/main/archive/0-1-26.tar.zst
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
      "$(pkg-config libcurl --libs --cflags)",
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
    host: text,
    port: text,
    dbname: text,
    user: text,
    password: text,
  )
}

// Establishes a connection to a DB.
// A connection is closed when the corresponding variable is not used.
// A connection is copied when the corresponding variable is used more than once.
define connect(c: conninfo): either(connection-error, connection)

// Executes a parameterized SQL command.
inline execute<a>(conn: &connection, com: command(a)): either(error, a)

// Executes a raw SQL command.
define batch(c: &connection, com: &text): either(error, unit)

// Executes the given action in a `begin ... commit`.
// `rollback` is executed instead of `commit` if `action` results in an error,
define with-transaction<a>(c: &connection, action: () -> either(error, a)): either(error, a)
```

### Command

```neut
// Represents a parameterized SQL command and a way to encode the result of the command.
// See the `Command` section of `Example` in this readme.
data command(a) {
| Command(
    template: &text,
    parameters: list(pair(&text, parameter)),
    result-encoder: (&table) -> either(encoder-error, a),
  )
}

// Represents the value of a placeholder in a SQL command.
data parameter {
| Integer(int)
| Float(float)
| Text(text)
| Time(text)
| Timestamp(time)
| Tuple(list(parameter))
}
```

### Table

```neut
// Represents a SQL table.
data table

// Gets the column size of a table.
define get-column-size(r: &table): int

// Gets the row size of a table.
define get-row-size(r: &table): int

// Gets the column number from a table by specifying column name.
define get-column-by-name(r: &table, column-name: &text): either(encoder-error, int)

// Reads the value at (row, column) in the given table as an integer.
inline encode-int(r: &table, row: int, column: int): either(encoder-error, int)

// Reads the value at (row, column) in the given table as a float.
inline encode-float(r: &table, row: int, column: int): either(encoder-error, float)

// Reads the value at (row, column) in the given table as a text.
inline encode-text(r: &table, row: int, column: int): either(encoder-error, text)

// Reads the value at (row, column) in the given table as a ISO8601 time.
inline encode-time(r: &table, row: int, column: int): either(encoder-error, time)
```

## Example

Suppose we created a table `user` as follows:

```sql
create table if not exists user (
  id bigint primary key generated always as identity,
  name text not null,
  created_at timestamptz not null
)
```

### Basics

The following code demonstrates a basic usage of `ars-postgres`:

```neut
define connect-then-insert-then-select(): unit {
  // Establishes a DB connection
  let conn-or-none = connect(my-conn) in
  match conn-or-none {
  | Left(Connection-Error(message)) =>
    printf("{}", [message])
  | Right(conn) =>
    let result on conn =
      with-transaction(conn, function () {
        // Executes SQL commands using a connection
        try _ = execute(conn, insert-user(name1, time1)) in
        try _ = execute(conn, insert-user(name2, time2)) in
        execute(conn, select-user(5))
      })
    in
    // (conn is closed here because it is discarded here)
    match result {
    | Right(user-list) =>
      // use `user-list: list(user)` as you like
      print("success\n")
    | Left(e) =>
      // use `e` to report an error
      print("error\n")
    }
  }
}

```

### Defining Commands

A command that performs `insert` can be defined, for example, as follows:

```neut
define insert-user(name: &text, now: time): command(unit) {
  Command of {
    // A SQL command with placeholders.
    template = "insert into user (name, created_at) values (__NAME__, __TIMESTAMP__)",

    // A mapping from placeholders to values.
    //
    // When executing this SQL command, a parameterized SQL command like the following is constructed:
    //   insert into user (name, created_at) values ($0, $1)
    // and this command is executed using libpq's `PQexecParams`, setting the parameter values as the
    // `paramValues` of `PQexecParams`.
    parameters = {
      [
        Pair("__NAME__", Text(*name)),
        Pair("__TIMESTAMP__", Timestamp(now)),
      ]
    },

    // Specifies how to encode the response of this command.
    // The response is ignored in this time since we're just inserting values.
    result-encoder = {
      function (_: &table) {
        Right(Unit)
      }
    },
  }
}
```

A command that performs `select` can also be defined, for example, as follows:

```neut
// Represents an user
data user {
| User(
    name: text,
    created-at: time,
  )
}

define select-user(limit: int): command(list(user)) {
  Command of {
    // A SQL command with placeholders (again).
    template = "select user.name, user.created_at from user where user.created_at > __EPOCH__ limit __LIMIT__",

    // A mapping from placeholders to values (again).
    parameters = {
      [
        Pair("__LIMIT__", Integer(limit)),
        Pair("__EPOCH__", Timestamp(time.time.Time of {seconds = 0, nanoseconds = 0})),
      ]
    },

    // Specifies how to encode the response of this command.
    // Here, we read the result of `select` and print them one by one.
    result-encoder = {
      function (res: &table) {
        // Get the row size of the response.
        let row-size = get-row-size(res) in

        // Gets the column indices by column names.
        try user-name-column = get-column-by-name(res, "name") in
        try user-created-at-column = get-column-by-name(res, "created_at") in

        // This `try-iterate` performs `function (r) {..}` for r = 0, ..., (row-size - 1) and creates a
        // list [f(0), f(1), ..., f(row-size - 1)].
        // If an iteration fails, `try-iteraet` also fails.
        try-iterate(row-size, function (r) {
          try name = encode-text(res, r, user-name-column) in
          try created-at = encode-time(res, r, user-created-at-column) in
          Right(User(name, created-at))
        })
      }
    },
  }
}
```

You may want to use `include-text` when specifying the `template` of a `Command`. See `source/test.nt` for working examples.
