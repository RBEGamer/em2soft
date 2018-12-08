## USING THE REST API


* eine visualisierung aller state gibt es unter http://127.0.0.1:3000/


* http://127.0.0.1:3000/rest/ventile listet alle ventile inkl states und IDs auf
* http://127.0.0.1:3000/rest/__ID__/get_state gibt den aktuellen state zurück dabei 0 -> geschlossen 1-> geöffnet *
* http://127.0.0.1:3000/rest/__ID__/get_info gibt alle informationen zum ventil an


* http://127.0.0.1:3000/rest/__ID__/set_state/__STATE__ setzt den status des ventils mit der ID dabei muss der state valide sein und in der valid_states vorkommen (in der regel 0 oder 1)


* *beim uic ventil ist der state 0 = P, 1 = ??, 2 =???