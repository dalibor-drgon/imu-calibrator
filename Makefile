
EMCC=emcc
INCLUDE_OPTS=-I src -I lib
SOURCES=src/optimizer.c src/calibrator.cpp

web: web/calibrator.js

web/calibrator.js: $(SOURCES)
	$(EMCC) $^ $(INCLUDE_OPTS) -s EXPORTED_RUNTIME_METHODS=["cwrap"] -O3 -o $@

example: example/calibrator

example/calibrator: $(SOURCES) example/main.cpp
	$(CXX) $^ $(INCLUDE_OPTS) -o $@

clean:
	rm -rf web/calibrator.js web/calibrator.wasm example/calibrator

