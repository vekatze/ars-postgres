drop table if exists student;

create table if not exists student (
  id bigint primary key generated always as identity,
  name text not null,
  my_time time not null,
  created_at timestamptz not null
)
