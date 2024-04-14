default:
	gcc -o ringmaster src/ringmaster.c src/utils.c src/question_handler.c src/parser.c src/inventory_handler.c src/execute.c

grade:
	python3 test/grader.py ./ringmaster test-cases