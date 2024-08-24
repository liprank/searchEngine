all: bin/DictProducer bin/PagePre bin/InvertProducer bin/searchServer bin/serverLRU bin/serverPool bin/serverTimer

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

bin/searchServer: bin/Dictionary.o bin/KeyRecommander.o bin/searchServer.o bin/WebPageQuery.o bin/SplitToolCppJieba.o
	g++ bin/Dictionary.o bin/KeyRecommander.o bin/searchServer.o -o bin/searchServer bin/WebPageQuery.o bin/SplitToolCppJieba.o -lworkflow -lwfrest -g
bin/Dictionary.o: src/Dictionary.cpp
	g++ -c src/Dictionary.cpp -I include -g -o bin/Dictionary.o
bin/KeyRecommander.o: src/KeyRecommander.cpp
	g++ -c src/KeyRecommander.cpp -I include -g -o bin/KeyRecommander.o
bin/searchServer.o: src/searchServer.cpp
	g++ -c src/searchServer.cpp -I include -g -o bin/searchServer.o
bin/WebPageQuery.o: src/WebPageQuery.cpp
	g++ -c src/WebPageQuery.cpp -I include -g -o bin/WebPageQuery.o

bin/serverLRU: bin/serverLRU.o bin/KeyRecommander.o bin/LRUCache.o bin/Dictionary.o
	g++ bin/serverLRU.o bin/KeyRecommander.o bin/LRUCache.o bin/Dictionary.o -o bin/serverLRU -lworkflow -lwfrest -g
bin/serverLRU.o: src/serverLRU.cpp
	g++ -c src/serverLRU.cpp -I include -g -o bin/serverLRU.o;
bin/LRUCache.o: src/LRUCache.cpp
	g++ -c src/LRUCache.cpp -I include -g -o bin/LRUCache.o

bin/serverPool: bin/serverPool.o bin/KeyRecommander.o bin/Dictionary.o bin/LRUCache.o 
	g++ bin/serverPool.o bin/KeyRecommander.o bin/Dictionary.o bin/LRUCache.o -o bin/serverPool -lworkflow -lwfrest -g
bin/serverPool.o: src/serverPool.cpp
	g++ -c src/serverPool.cpp -I include -g -o bin/serverPool.o

bin/serverTimer: bin/serverTimer.o bin/KeyRecommander.o bin/Dictionary.o bin/LRUCache.o
	g++ bin/serverTimer.o bin/KeyRecommander.o bin/Dictionary.o bin/LRUCache.o -o bin/serverTimer -lworkflow -lwfrest -g
bin/serverTimer.o: src/serverTimer.cpp
	g++ -c src/serverTimer.cpp -I include -g -o bin/serverTimer.o