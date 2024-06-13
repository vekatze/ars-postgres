select
  author.name,
  author.some_value,
  author.created_at
from
  author
where
  author.created_at > __EPOCH__
limit
  __LIMIT__
