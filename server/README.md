# websocket server
Basic C linux websocket server

Install first setup wslay :

			Dependancies :
							cunit >= 2.1
							nettle >= 2.4
							python-sphinx for man generation

			Setup nettle-3.3:
							./install_nettle


			Setup wslay :
							./install_wslay

Caution --> it is possible that the command make install end unexpectedly
--> Don't worry you will still be able to run the program using
--> The command :
-->					make static

Compile :
			make

			make SIZE=<max_player_count> target
		or
			make static

Run :
			./ws_server
		or
			./ws_server <port_number>


Then run the /client/index.html or go to http://www.wsclient.elhmn.com
