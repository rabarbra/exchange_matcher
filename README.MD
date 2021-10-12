# LIMIT ORDERS MATCHING ENGINE

# Building

```bash
git clone https://github.com/rabarbra/exchange_matcher.git
cd exchange_matcher
make all
```
# Usage

Running:
```bash
./exchange_matcher -i input.txt -o output.txt
```
Arguments:
* `-i` - input file, default is stdin
* `-o` - output file, default is stdout

Input data format:
* `O`,`order id (uint)`,`order side - S(ell) or B(uy)`,`quantity (uint)`,`price (float, precision = 2)`
* `C`,`order id (uint)` - cancell order
- Examples:
* `O,1,S,2,30.52` - sell order, order id = 1, quantity = 2, price = 30.52
* `O,2,B,45,200.4` - buy order, order id = 2, quantity = 46, price = 2.4 
* `C,2` - cancel order with order id 2

Output data format:
* `T`,`trade id (uint)`,`S(ell) or B(uy)`,`older order id (uint)`,`newer order id (uint)`,`quantity (uint)`,`price (float .2)`
* `X`,`order id` - order has been cancelled
- Examples:
* `T,1,S,1,2,2,30.52` - Sell trade beetween orded 1 and order 2, quantity = 2, price = 30.52
* `X,2` - Order 2 has been cancelled

# Data structures and time performance

There are used two sorted linked lists for buy and sell orders.
Orders get filled in O(1) time, are placed if no match - in O(n) time, are cancelled in O(n).

# TO DO

* Test coverage
* Better gateways for input / output - read and write any defined buffer, not line by line.
* Better error processing and graceful shutdown logic.
* Maybe another data structure for placed limit orders with better time performance for place and delete operations.