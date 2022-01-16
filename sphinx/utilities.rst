Utilities
=========

- :ref:`exhale_function_getTime_8cpp_1a0e47c74b5a3e6246b9936d96ff4c66b1` 
  
Math
----

- :ref:`exhale_class_classO3DS_1_1Vector4`
- :ref:`exhale_class_classO3DS_1_1Matrix`


Apps
----

**Repeater**

The repeater (apps/Repeater) listens for incoming data using a ServerPair
connector and publishes on using a Publish/Subscribe.  This can be run 
on a public internet node to allow a source to broadcast data out to many
clients without needing to configure firewall ports or set up a vpn.

See apps/repeater/main.cpp for more information

**FbxStream**

This application as a way to broadcast a fbx file playing on loop.  It
is useful for testing connectivity.

    usage: fbxstream file.fbx protocol url

Protocol can be pub, client or server

see apps/FbxStream/main.cpp

