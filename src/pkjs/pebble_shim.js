
function sendAppMessage(d, succ, fail){
    console.log(d);
    succ();
}

function getWatchToken(){
    return "dummy_watch_token";
}

module.exports.sendAppMessage = sendAppMessage;
module.exports.getWatchToken = getWatchToken;