var xhrRequest = function (url, type, callback) {
	var xhr = new XMLHttpRequest();
	xhr.onload = function () {
		callback(this.responseText);
	};
	xhr.open(type, url);
	xhr.send();
};

function locationSuccess(pos) {
	// Construct URL
	var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
		pos.coords.latitude + '&lon=' + pos.coords.longitude + 
		'&APPID=00ce7f5f1a49f1d44e2f34ad820a9462';

	// Send request to OpenWeatherMap
	xhrRequest(url, 'GET', 
		function(responseText) {
			// responseText contains a JSON object with weather info
			var json = JSON.parse(responseText);

			// Temperature in Kelvin requires adjustment
			var temperature = Math.round(json.main.temp - 273.15);
			console.log('Temperature is ' + temperature);

			// Conditions
			var conditions = json.weather[0].main;      
			console.log('Conditions are ' + conditions);
			
			var dictionary = {
				'WEATHER_TEMPERATURE': temperature
			};

			// Send to Pebble
			Pebble.sendAppMessage(dictionary, 
				function(e) {
					console.log('Weather info sent to Pebble successfully!');
				},

				function(e) {
					console.log('Error sending weather info to Pebble!');
				}
			);
		}
	);
}


function locationError(err) {
	console.log('Error requesting location!');
}

function getWeather() {
	navigator.geolocation.getCurrentPosition(
		locationSuccess,
		locationError,
		{timeout: 15000, maximumAge: 60000}
	);
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
	function(e) {
		console.log('PebbleKit JS ready!');
	}
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
	function(e) {
		console.log('AppMessage received in JS! - Getting weather...');
		getWeather();
	}
);

Pebble.addEventListener('showConfiguration',
	function(e) {
		console.log('showConfiguration event');
		Pebble.openURL('https://dl.dropboxusercontent.com/u/8602374/Zelda_GoT_Config.html');
	}
);

Pebble.addEventListener('webviewclosed',
	function(e) {
		console.log('webviewclosed event');
		
		//Get JSON dictionary
		var configuration = JSON.parse(decodeURIComponent(e.response));
		console.log('Configuration window returned: ' + JSON.stringify(configuration));
 
		//Send to Pebble, persist there
		Pebble.sendAppMessage(
			{'UNIT_TEMPERATURE': configuration.tunit},
			function(e) {
				console.log('Sending settings data...');
			},
			function(e) {
				console.log('Settings feedback failed!');
			}
		);
	}
);
