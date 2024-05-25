select
  student.name,
  student.created_at
from
  student
where
  student.created_at > __EPOCH__
limit
  __LIMIT__
