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
			
Workaround:
		-->While compiling you might receive this error :

			 error while loading shared libraries: 
			libwslay.so.0 or nettle.so.0 : cannot open shared object file: No such 
			file or directory

		--> This has something to do with dynamic libraries just run :

			make linker

		and
			make

		--> If you still have the same issue just send me an email :)

Run :
			./ws_server
		or
			./ws_server <port_number>


Zboob :
	A tous les zboob pour le pi a une plage de 100 ports ouverte
	elle va de 10000 a 10100
	donc si vous avez acces au pi il suffit de lancer le server avec un
	de ces ports et de renseigner les addresse Ip et port du pi au client

Then run the /client/index.html or go to http://www.wsclient.elhmn.com
