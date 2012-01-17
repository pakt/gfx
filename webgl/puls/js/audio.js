var context = new (window.AudioContext || window.webkitAudioContext)();
var source;
var analyser;
var processor;

var bin0 = 0.0;
var bin1 = 0.0;
var bin2 = 0.0;

function loadSample(url) {
    var request = new XMLHttpRequest();
    request.open("GET", url, true);
    request.responseType = "arraybuffer";

    request.onload = function() {
      source.buffer = context.createBuffer(request.response, false);
      source.looping = true;
      source.noteOn(0);
    }

    request.send();
}

function audio_init() {
    source = context.createBufferSource();
    processor = context.createJavaScriptNode(2048 /*bufferSize*/, 1 /*num inputs*/, 1 /*numoutputs*/);
    processor.onaudioprocess = processAudio;
    analyser = context.createAnalyser();

    source.connect(context.destination);
    source.connect(analyser);

    analyser.connect(processor);
    processor.connect(context.destination);

    loadSample("hybrid.mp3");
    //source.noteOn(0);
    //setTimeout(disconnect, source.buffer.duration * 1000);

}

function processAudio(e) {
    var freqByteData = new Uint8Array(analyser.frequencyBinCount);
    analyser.getByteFrequencyData(freqByteData);
    
    var total = freqByteData.length;
    var idx = [100, 400, 700];
    var n = 20;
    var bins = [];
    var bc = 3;
    for(var i=0;i<bc;i++) bins[i] = 0;
    for(var i=0;i<bc;i++){
        var ind = idx[i];
        for(var j=ind;j<ind+n;j++){
            bins[i] += freqByteData[j];
        }
    }
    for(var i=0;i<bc;i++) bins[i] /= n*256;
    bin0 = bins[0];
    bin1 = bins[1];
    bin2 = bins[2];
    //console.log(total, bins);
    
}


function audio_disconnect() {
    try{
        source.noteOff(0);
        source.disconnect(0);
        processor.disconnect(0);
        analyser.disconnect(0);
    }
    catch(e){
    }
}

