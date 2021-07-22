var Twit = require('twit');
var config = require('./config');

var T = new Twit(config);
var randomnum = Math.floor(Math.random()*100000);
var txt = 'here is a random number ' + randomnum + ' #doorstatus from RPI';
tweetIt(txt);
var status = 'opened';

setInterval(() => {
	if(status=='opened'){
		status = 'closed';
	}
	else{
		status = 'opened';
	}			
	var randomnum = Math.floor(Math.random()*100000);
	var txt = '#doorstatus ' + randomnum + '$$$$' + status;
	tweetIt(txt);
}, 20000);

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