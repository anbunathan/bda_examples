var Twit = require('twit');
var config = require('./config');
var T = new Twit(config);
const pigpio = require('pigpio-client').pigpio({
    host: 'localhost',
    port: 8888
});

const ready = new Promise((resolve, reject) => {
    pigpio.once('connected', resolve);
    pigpio.once('error', reject);
});

function wait(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

ready.then(async (info) => {
const button = pigpio.gpio(18);
await button.modeSet('input');
button.pullUpDown(2)
button.notify((level, tick)=> {
    console.log(`Button changed to ${level} at ${tick} usec`)
  });
  
// control an motor on GPIO 17
const motor = pigpio.gpio(17);
await motor.modeSet('output');
let pulseWidth = 500;
var status = 'opened';

setInterval(() => {
  //pulseWidth += increment;
  if (pulseWidth == 500) {
    pulseWidth = 1500;
    status = 'opened';
  } else if (pulseWidth == 1500) {
    pulseWidth = 500;
    status = 'closed';
  }
  motor.setServoPulsewidth(pulseWidth);
  var randomnum = Math.floor(Math.random()*100000);
  var txt = '#doorstatus ' + randomnum + '$$$$' + status;
  tweetIt(txt);
}, 5000);

}).catch(console.error);

function tweetIt(txt){		
	var tweet = {
		status: txt
	}

	T.post('statuses/update', tweet, tweeted);

	function tweeted(err, data, response){
		if(err){
			console.log("Some thing went wrong");
		} else{
			console.log("Tweeted");
		}	
	}
}

