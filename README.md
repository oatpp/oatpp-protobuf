# oatpp-protobuf

**Alpha**  
**Since oatpp 1.2.0**

Protobuf messages integration with oatpp object-mapping framework.

Now you can use protobuf messages as a regular oatpp DTOs!

## Usage

Let's say we have a .proto file:

```proto
syntax = "proto3";

message User {
    enum Role {
        ADMIN = 0;
        GUEST = 1;
    }
    string name = 1;
    Role role = 2;
}
```

### In Endpoint

```cpp
ENDPOINT("POST", "createUser", createUser, 
         BODY_DTO(oatpp::protobuf::Object<User>, user)) 
{
  auto name = user->name();
  auto role = user->role();
  ...
  return createResponse(Status::CODE_200, "OK");
}
```

```cpp
ENDPOINT("GET", "getUser", getUser) 
{
  oatpp::protobuf::Object<test::User> user = std::make_shared<test::User>();
  user->set_name("Ivan");
  user->set_role(User_Role_GUEST);
  return createDtoResponse(Status::CODE_200, user);
}
```

Output:

```json
{
  "name": "Ivan",
  "role": "GUEST"
}
```
