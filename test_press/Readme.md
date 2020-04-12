## NOTE
When pressing, we often need to solve many problems such as threads or transfer protocol and so on, then we could focus on testing ofour projects; so one press framework is necessary for us.

## INFO
### 1. Easy to add transfer protocol
Register is used of our press framework, so if you want anthor protocol, it's easy to add it:
1. Inherit `BusiClient` and rewrite function of SendAndRecv
2. Register this new class
then you could use it by this register name
Note: we provide http and rpc transfer protocol which you could use directly

### 2. Easy to add press testing
Register also is used to add new press testing:
1. Inherit `PressObject` and rewirte `ExecBody` which is your press
2. Register this press object
then you could do this press using register name of command
