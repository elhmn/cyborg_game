var		wsclient = function ()
{
	var		ws_onopen = function (event)
	{
		document.getElementById("after-connect").style.display = "initial";
		document.getElementById("con-btn").style.display = "none";
		console.log("Connection established !");
	};

	var		ws_onclose = function (event)
	{
		var code = event.code;
		var reason = event.reason;
		var wasClean = event.wasClean;

		console.log("Socket connection closed !");
		if (wasClean)
		{
			console.log("Connection closed normally !");
			document.getElementsByTagName("body")[0].innerHTML += "Connection closed normally ! : " + 
			+ reason + "(Code : " + code + ") !";
			document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
		}
		else
		{
			console.log("Connection closed with message : " + 
			+ reason + "(Code : " + code + ") !");
			document.getElementsByTagName("body")[0].innerHTML += "Connection closed with message : " + 
			+ reason + "(Code : " + code + ") !";
			document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
		}
	};

	var		ws_onerror = function (event)
	{
		console.log("Socket Error !");
		var elem = document.getElementsByTagName("body");
		document.getElementsByTagName("body")[0].innerHTML += "Wrong socket Error : ";
		document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
	};

	var		ws_onmessage = function (event)
	{
		console.log("je test ! on message");
		if (gameState == e_gameState.waitRoom)
		{
			if (typeof event.data === "string")
			{
				console.log(event.data);
				var data = event.data.split("/");
				console.log(data);
				if (data && data.length > 0)
				{
					player_id = parseInt(data[0]);
					document.getElementById('head-id').innerHTML = player_id + 1;
					console.log(id = player_id);
					var list = data[data.length - 1].split(";");
					// list = list.filter(v=>v!='');
					// console.log(list);
					var room = document.getElementById('room');
					room.innerHTML = "";
					for (var i = 0; i < list.length; i++)
					{
						if (list[i])
						{
							room.innerHTML += '<li class="placement-hr"><span style="margin:auto">P'
										+ (i + 1) + ' - ' + list[i] + ' - 0 pts</span><span class="color-yel" style="display:none">(master)</span></li>'
						}

					}

				}
			}
		}
	};

	var		ws_request = function ()
	{
		if (ws.readyState === WebSocket.OPEN)
		{
			ws.send("bonjour");
		}
	};

	var uri = "ws://";
	var host_addr = window.prompt("Enter your IP adress");
	var host_port = window.prompt("Enter port number");

	var player_id = -1;
	var	e_gameState = (function () {
		var n = 0;
		return ({
					waitRoom : n++,
					gamecanStart : n++,
					gameStart : n++,
					gameChoose : n++,
					gameCompare : n++,
					gameResult : n++,
					gameEditRole : n++,
					gameEnd : n++
		});
	})();

	var gameState = e_gameState.waitRoom;
	console.log(e_gameState);
	console.log(gameState);

	if (isNaN(host_port))
	{
		console.log("Error : wrong port number");
		return ;
	}
	uri = uri + host_addr + ":" + host_port; /* not used for the moment */
	try
	{
		var		ws = new WebSocket(uri);
	}
	catch (e)
	{
		document.getElementsByTagName("body")[0].innerHTML += "Wrong socket Uri " + e;
		document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
	}
	console.log(ws);
	if (ws)
	{
		ws.onopen = ws_onopen;
		ws.onclose = ws_onclose;
		ws.onerror = ws_onerror;
		ws.onmessage = ws_onmessage;
		setInterval(ws_request, 50);
	}
};

var		conBtnAddEventListener = function ()
{
	document.getElementById('con-btn').addEventListener('click', function ()
	{
		wsclient();
	});
};

var websocket_test = function ()
{
	if (window.WebSocket)
	{
		console.log("Websocket supported !");
	}
	else
	{
		console.log("Websocket not supported !");
		window.alert("Consider update you browser for reacher experience !!");
		//implement a fallback solution
	};
}

websocket_test();
conBtnAddEventListener();