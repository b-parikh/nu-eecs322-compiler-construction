all: L1_lang L2_lang L3_lang IR_lang LA_lang LB_lang

L1_lang:
	cd L1 ; make 

L2_lang:
	cd L2 ; make

L3_lang:
	cd L3 ; make

IR_lang:
	cd IR ; make

LA_lang:
	cd LA ; make

LB_lang:
	cd LB ; make

LC_lang:
	cd LC ; make

LD_lang:
	cd LD ; make

LE_lang:
	cd LE ; make

framework:
	./scripts/framework.sh

homework:
	./scripts/homework.sh

generate_tests: all
	./scripts/generateTests.sh

clean:
	rm -f *.bz2 ; cd L1 ; make clean ; cd ../L2 ; make clean ; cd ../L3 ; make clean ; cd ../IR ; make clean ; cd ../LA ; make clean ; cd ../LB ; make clean ; cd ../C ; make clean ; 
