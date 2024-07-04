compile: 
	emcc main.cpp -o app.js -sEXPORTED_RUNTIME_METHODS=ccall,cwrap