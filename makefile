lex: lexer.cc
	g++ inputbuf.cc lexer.cc
	./a.out < input.txt

	#  ./test1.sh > test.txt
	# How to run test command