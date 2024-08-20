all: bin/DictProducer bin/PagePre bin/InvertProducer

bin/DictProducer: bin/DictProducer.o bin/SplitToolCppJieba.o bin/Configuration.o
	g++ bin/DictProducer.o bin/SplitToolCppJieba.o bin/Configuration.o -o bin/DictProducer
bin/DictProducer.o: src/DictProducer.cpp
	g++ -c src/DictProducer.cpp -I include -g -o bin/DictProducer.o
bin/SplitToolCppJieba.o: src/SplitToolCppJieba.cpp
	g++ -c src/SplitToolCppJieba.cpp -I include -g -o bin/SplitToolCppJieba.o
bin/Configuration.o: src/Configuration.cpp
	g++ -c src/Configuration.cpp -I include -g -o bin/Configuration.o

bin/PagePre: bin/myxml.o bin/tinyxml2.o bin/PagePre.o bin/Configuration.o
	g++ bin/myxml.o bin/tinyxml2.o bin/PagePre.o bin/Configuration.o -o bin/PagePre
bin/myxml.o: src/myxml.cpp
	g++ -c src/myxml.cpp -I include -g -o bin/myxml.o
bin/tinyxml2.o: src/tinyxml2.cpp
	g++ -c src/tinyxml2.cpp -I include -g -o bin/tinyxml2.o
bin/PagePre.o: src/PageLibPreprocessor.cpp
	g++ -c src/PageLibPreprocessor.cpp -I include -g -o bin/PagePre.o

bin/InvertProducer: bin/InvertProducer.o bin/SplitToolCppJieba.o bin/tinyxml2.o
	g++ bin/InvertProducer.o bin/SplitToolCppJieba.o bin/tinyxml2.o -o bin/InvertProducer
bin/InvertProducer.o: src/InvertProducer.cpp
	g++ -c src/InvertProducer.cpp -I include -g -o bin/InvertProducer.o 