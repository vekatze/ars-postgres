drop table if exists author;

create table if not exists author (
  id bigint primary key generated always as identity,
  name text not null,
  some_value real not null,
  my_time time not null,
  created_at timestamptz not null
)
