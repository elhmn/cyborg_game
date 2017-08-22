var host_addr = '127.0.0.1'
var host_port = '8080';

var		wsclient = function ()
{
	var		newGameBtnAddEventListener = function ()
	{
		document.getElementById('div-btn-restart').addEventListener('click', function ()
		{
				
			// var res = document.getElementById('btn-restart');
			console.log("click Test !");
			window.location.reload();
		});
	};


	var		sendBtnAddEventListener = function ()
	{
		var todo = function ()
		{
			if (gameState == e_gameState.gameEnd)
				return ;
			choice = document.getElementById('choice').value;
			var res = document.getElementById('btn-send-msg');
			if (!choice || isNaN(choice))
			{
				res.innerHTML = "You must enter a Number";
				document.getElementById('btn-send').innerHTML = "send";
				return ;
			}
			console.log(choice);
			if (parseInt(choice) < parseInt(limMin) || parseInt(choice) > parseInt(limMax))
			{
				res.innerHTML = "number out of range [" + limMin + ", "+ limMax +"]";
				document.getElementById('btn-send').innerHTML = "send";
				return ;
			}
			document.getElementById('btn-send').innerHTML = "sending...";
			res.innerHTML = "";
			updateMsg(player_id + "/ws/guess/" + choice);
			console.log(player_id + "/ws/guess/" + choice); //Debug
		};

		document.getElementById('div-btn-send').addEventListener('click', function ()
		{
			todo();
		});

		document.getElementById('choice').addEventListener('keypress', function (e)
		{
			if (e.keyCode == 13)
			{
				todo();
			}
		});
	};

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
			document.getElementById("error").innerHTML += "Connection closed normally ! : " + 
			+ reason + "(Code : " + code + ") !";
			document.getElementById("error").innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
		}
		else
		{
			console.log("Connection closed with message : " + 
			+ reason + "(Code : " + code + ") !");
			document.getElementById("error").innerHTML += "Connection closed with message : " + 
			+ reason + "(Code : " + code + ") !";
			document.getElementById("error").innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
		}
	};

	var		ws_onerror = function (event)
	{
		console.log("Socket Error !");
		var elem = document.getElementsByTagName("body");
		document.getElementsByTagName("body")[0].innerHTML += "Wrong socket Error : ";
		document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
	};

	var		getConList = function (data)
	{
		if (data[1] == "ws" && data[2] == "list")
		{
			var list = data[data.length - 1].split(";");

			player_id = parseInt(data[0]);
			document.getElementById('head-id').innerHTML = player_id + 1 + " over " + list.length;
			// list = list.filter(v=>v!='');
			// console.log(list);
			var room = document.getElementById('room');
			room.innerHTML = "";
			for (var i = 0; i < list.length; i++)
			{
				if (list[i])
				{
					room.innerHTML += '<li class="placement-hr"><span>P'
								+ (i + 1) + ' - ' + list[i] + ' - 0 pts</span><span class="color-yel" style="display:none">(ready)</span></li>'
				}
			}
		}
	};

	var		getChallenger =  function (data)
	{
		if (data[1] == "ws" && data[2] == "challenger")
		{
			var		fun_play =  function ()
			{
				document.getElementById("btn-play-msg").innerHTML = "Ready ! Waiting for other players...";
				document.getElementById("btn-play").innerHTML = "starting...";
				player_ready = true;
				updateMsg(player_id + "/ws/ready");
				document.getElementById("div-btn-play").removeEventListener('click', fun_play);
			};

			var		fun_wait =  function ()
			{
				if (player_ready == false)
					document.getElementById("btn-play-msg").innerHTML = "You need at least 2 players !";
			};

			if (data[3] == "1")
			{
				if (player_ready == false)
				{
					document.getElementById("btn-play").innerHTML = "play";
					document.getElementById("div-btn-play").removeEventListener('click', fun_wait);
					document.getElementById("div-btn-play").addEventListener('click', fun_play);
				}
			}
			else
			{
				if (player_ready == false)
				{
					document.getElementById("btn-play").innerHTML = "waiting for connection...";
					document.getElementById("div-btn-play").removeEventListener('click', fun_play);
					document.getElementById("div-btn-play").addEventListener('click', fun_wait);
				}
			}
		}
	};

	var		getReady = function (data)
	{
		if (data[1] == "ws" && data[2] == "ready")
		{
			var list = data[data.length - 1].split(";");
			var room = document.getElementById('room');
			var e = room.querySelectorAll("li span:last-child");
			for (var i = 0; i < list.length; i++)
			{
				console.log(e[i]);
				if (e[i])
				{
					if (list[i] == 1)
					{
						e[i].style.display = "flex";
					}
					else
					{
						e[i].style.display = "none";
					}
				}
			}
		}
	}

	var		getAllReady = function (data)
	{
		console.log("ALL READY");
		if (data[1] == "ws" && data[2] == "allready")
		{
			var room = document.getElementById('room');
			var game = document.getElementById('game');
			game.style.display = "flex";
			var e = room.querySelectorAll("li span:last-child");
			var setCon = document.querySelector("#set-connect");
			var play = document.querySelector("#div-btn-play");
			var playRes = document.querySelector("#btn-play-msg");
			var playerList = document.querySelectorAll("#room>li span:first-child");
			play.style.display = "none";
			playRes.style.display = "none";
			setCon.style.display = "none";
			console.log(playerList);
			for (var i = 0; i < e.length; i++)
			{
				e[i].style.display = "none";
			}
			for (var i = 0; i < playerList.length; i++)
			{
				playerList[i].style.margin = "auto";
			}
			gameState = e_gameState.gameStart;
			updateMsg(player_id + "/ws/start");
		}
	}

	var		fs_waitRoom = function (event)
	{
		if (typeof event.data === "string")
		{
			console.log(event.data);
			var data = event.data.split("/");
			console.log(data);
			if (data && data.length > 0)
			{
				
				getConList(data);
				getChallenger(data);
				getReady(data);
				getAllReady(data);
			}
		}
	};

	var		getLimits = function (data)
	{
		if (data[1] == "ws" && data[2] == "limits")
		{
			var list = data[data.length - 1].split(";");
			if (list && list.length == 2)
			{
				document.getElementById("number-min").innerHTML = limMin = list[0];
				document.getElementById("number-max").innerHTML = limMax = list[1];
			}
			// var room = document.getElementById('room');
			// var e = room.querySelectorAll("li span:last-child");
		}
	}

	var		getResult = function (data)
	{
		if (data[1] == "ws"
			&& (data[2] == "found"
				|| data[2] == "outofrange"
				|| data[2] == "guess"
				))
		{
			var res = document.getElementById('btn-send-msg');
			if (data[2] == "outofrange")
			{
				res.innerHTML = "number out of range [" + limMin + ", "+ limMax +"]";
				return ;
			}
			else if (data[2] == "guess")
			{
				var tmp = document.getElementById('game-text-result');
				if (tmp.style.display == "none")
				{
					tmp.style.display = "initial";
				}
				if (data[3] == "1")
					document.getElementById('number-state').innerHTML = "greater";
				else if (data[3] == "0")
					document.getElementById('number-state').innerHTML = "smaller";
			}
			else if (data[2] == "found")
			{
				updateMsg(player_id + "/ws/winner");
				console.log(player_id + "/ws/winner");
			}
			document.getElementById('btn-send').innerHTML = "send";
		}
	}

	var		getWinner = function (data)
	{
		if (data[1] == "ws" && data[2] == "winner")
		{
			var list = data[data.length - 1].split(";");
			if (list && list.length == 2)
			{
				var tmp = document.getElementById('game-text-result');
				if (tmp.style.display == "none")
				{
					tmp.style.display = "initial";
				}
				tmp.innerHTML = 'Player ' + (parseInt(list[0]) + 1) + ' wins ! Cyborg number was <span class="color-yel">'+ list[1] + '</span>!'
				gameState = e_gameState.gameEnd;
				document.getElementById('div-btn-restart').style.display = "flex";
				updateMsg(player_id + "/ws/end");
				console.log(player_id + "/ws/end");
			}
		}
	}

	var		fs_gameStart = function (event)
	{
		if (typeof event.data === "string")
		{
			var data = event.data.split("/");
			console.log(data);
			if (data && data.length > 0)
			{
				getLimits(data);
				getResult(data);
				getWinner(data);
			}
		}
	};

	var		fs_gameEnd = function (event)
	{
		if (typeof event.data === "string")
		{
			var data = event.data.split("/");
			console.log(data);
			if (data && data.length > 0)
			{
				// getLimits(data);
				// getResult(data);
			}
		}
	};

	var		canConnect = function (event)
	{
		if (typeof event.data === "string")
		{
			var data = event.data.split("/");
			console.log(data);
			if (data && data.length > 0)
			{
				player_id = parseInt(data[0]);
				console.log(id = player_id);
				if (data[1] == "ws" && data[2] == "full")
				{
					document.getElementsByTagName("body")[0].innerHTML += "The game room is full, retry later !";
					document.getElementsByTagName("body")[0].innerHTML += '<br/><br/><span style="color:red">RELOAD THE PAGE</span>';
					ws.close();
					return (false);
				}
			}
		}
		return (true);
	}

	var		ws_onmessage = function (event)
	{
		console.log(event.data);
		canConnect(event);
		var fs_tab = {
			waitRoom : fs_waitRoom
		};
		//call the corresponding state function
		gameState[gameState.length - 1](event);
	};

	var		ws_request = function ()
	{
		if (ws.readyState === WebSocket.OPEN)
		{
			ws.send(sendingMsg);
			updateMsg(stdSendingMsg);
		}
	};

	var uri = "ws://";
	
	var stdSendingMsg = "hello";
	var	sendingMsg = stdSendingMsg;
	var updateMsg = function (msg)
	{
		sendingMsg = msg;
	};

	var player_id = -1;
	var player_ready = false;
	var limMin = 0;
	var limMax = 0;
	var choice = -1;
	var	e_gameState = (function () {
		var n = 0;
		return ({
					waitRoom : [n++, fs_waitRoom],
					gameStart : [n++, fs_gameStart],
					gameEnd : [n++, fs_gameEnd]
		});
	})();

	var gameState = e_gameState.waitRoom;
	console.log(e_gameState);
	console.log(gameState);

	sendBtnAddEventListener();
	newGameBtnAddEventListener();
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
		setInterval(ws_request, 1);
	}
};

var		setBtnAddEventListener = function ()
{
	console.log("Set ip / port");
	document.getElementById('div-btn-set').addEventListener('click', function ()
	{
		console.log("Set ip / port");
		host_addr = window.prompt("Enter your IP adress");
		host_port = window.prompt("Enter port number");
		if (isNaN(host_port))
		{
			console.log("Error : wrong port number");
			return ;
		}
		document.getElementById('btn-set-msg').innerHTML = "ip : " + host_addr + "; port : " + host_port;
		console.log("Set ip / port");
	});
};

var		conBtnAddEventListener = function ()
{
	setBtnAddEventListener();	
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
