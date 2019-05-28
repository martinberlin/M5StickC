<?php
/**
 * Very straight-forward Curl example to send an Order to your Mesh-Lights
 * Documentation: 
 * https://docs.espressif.com/projects/esp-mdf/en/latest/api-guides/mlink.html
 */
$ch = curl_init();

curl_setopt($ch, CURLOPT_URL, "http://esp32_mesh.local/device_request" );

curl_setopt($ch, CURLOPT_POST, 1);

curl_setopt($ch, CURLOPT_HTTPHEADER,  [
	'Content-Type: application/json',
	// Replace this with your Light ID after making a mesh_info call:
	'Mesh-Node-Mac:3c71bf9d6ab4,3c71bf9d6980'
	]);

curl_setopt($ch, CURLOPT_POSTFIELDS,    
	'{"request": "set_status",
	"characteristics": [{
		"cid": 1,
		"value": 0
	}]}' );
$result = curl_exec ($ch);
echo $result."\n";
