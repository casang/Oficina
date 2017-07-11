// This file and the HTML associated with it were adapted from the
// Realtime JavaScript Evaluator at
// http://www.teria.com/~koseki/memo/javascript/realtime_eval.html

var buffer = "";

function evalOnce() {
    try {
        var code = document.getElementById("code").value;
        var result = eval(code);
        buffer += result;
        flush();
    } catch (ex) {
        buffer = ex;
        flush();
    }
}

function clear()  {
    var out = document.getElementById("out");
    out.value = "";
}

function print(str)  {
    buffer += str + "\n";
}

function flush()  {
    var out = document.getElementById("out");
    out.value = buffer;
    buffer = "";
}
