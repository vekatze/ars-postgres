# ars-postgres

`ars-postgres` is a PostgreSQL (libpq) binding for the [Neut](https://vekatze.github.io/neut/) programming language.

## test

```sh
docker compose up -d
export PKG_CONFIG_PATH=/opt/homebrew/opt/libpq/lib/pkgconfig:$PKG_CONFIG_PATH
neut build test --execute
# => creates tables, inserts rows, selects rows, and prints them
```

Please refer to `source/test.nt` to see how to use this module (for now).
