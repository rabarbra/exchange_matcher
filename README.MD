# MARKET ORDERS MATCHING ENGINE

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

Input data structude:
* `O`,`order id (uint)`,`order side - S(ell) or B(uy)`,`quantity (uint)`,`price (float, precision = 2`
* `C`,`12` - cancell order 12

* `O,1,S,2,30.52` - sell order, order id = 1, quantity = 2, price = 30.52
* `O,2,B,45,200.4` - buy order, order id = 2, quantity = 46, price = 2.4 
* `C,2` - cancel order with order id 2

Output data structure:
* `T,1,S,1,2,2,30.52` - Sell trade beetween orded 1 and order 2, quantity = 2, price = 30.52
* `X,2` - Order 2 has been cancelled
