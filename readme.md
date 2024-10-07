# ars-postgres

`ars-postgres` is a PostgreSQL binding for the [Neut](https://vekatze.github.io/neut/) programming language.

## Installation

### Module

```sh
neut get ars-postgres https://github.com/vekatze/ars-postgres/raw/main/archive/0-1-14.tar.zst
```

### PostgreSQL and libpq

Install PostgreSQL and libpq:

```sh
# Debian
apt install postgresql-server-dev-all

# Ubuntu
apt install postgresql-server-dev-all

# macOS
brew install postgresql@17 libpq
# (... then add /opt/homebrew/opt/postgresql@17/bin to your PATH)
```

Ensure that the following two commands run successfully:

- `pg_config --includedir`
- `pg_config --includedir-server`

### pkg-config

Install `pkg-config`:

```sh
# Debian
apt install pkg-config

# Ubuntu
apt install pkg-config

# macOS
brew install pkg-config
```

Ensure that the following command runs successfully:

- `pkg-config libpq --libs --cflags`

Add the below to your app's `build-option`:

```ens
  target {
    test {
      main "test.nt",
      build-option [                           //
        "$(pkg-config libpq --libs --cflags)", // ← add this
      ],                                       //
    },
  },
```

On macOS, set:

```sh
export PKG_CONFIG_PATH=/opt/homebrew/opt/libpq/lib/pkgconfig:$PKG_CONFIG_PATH
```

## Test

```sh
# macOS
docker compose up -d
neut build test --execute
# => creates tables, inserts rows, selects rows, and prints them
```

Please refer to `source/test.nt` to see how to use this module (for now).
