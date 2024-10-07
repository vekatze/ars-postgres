# ars-postgres

`ars-postgres` is a PostgreSQL binding for the [Neut](https://vekatze.github.io/neut/) programming language.

## Installation

### Module

```sh
neut get ars-postgres https://github.com/vekatze/ars-postgres/raw/main/archive/0-1-15.tar.zst
```

### PostgreSQL and libpq

Install libpq and pkg-config:

```sh
# Debian
apt install libpq-dev pkg-config

# Ubuntu
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

Add the below to your app's `build-option`:

```ens
  target {
    your-app {
      main "your-app.nt",
      build-option [                           //
        "$(pkg-config libpq --libs --cflags)", // ← add this
      ],                                       //
    },
  },
```

## Test

```sh
# macOS
docker compose up -d
neut build test --execute
# => creates tables, inserts rows, selects rows, and prints them
```

Please refer to `source/test.nt` to see how to use this module (for now).
