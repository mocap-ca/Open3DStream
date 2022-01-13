Open 3D Stream
================

Alastair Macleod, 2022

The goal of this project is to provide a simple and unified way to stream realtime
3d content between devices, applications and locations.

Currently there are a number of standards and proprietary applications that can be
used to stream realtime. Getting devices to interface with applications or applications
to interface with each other often requires writing custom code to connect protocols
or API’s. It would be much easier if a standard could be adopted for transmitting
and receiving realtime data. This project seeks to create this harmony by creating
awareness of existing standards where they exist and are useful, proposing new
standards, documenting examples and providing test cases.

While theoretically any time sampled or event data that is taken from the real world
and needs to be transmitted is considered, the focus of this project is on data
captured by mobile devices (AR Kit, Accelerometer, GPS), Motion/Performance capture
and Virtual Production (Camera Intrinsic/Extrinsic)

.. doxygenindex::
    :allow-dot-graphs:

.. toctree::
   :maxdepth: 1
   :caption: Contents:

   plugins
   doxyindex
   connectors
   utilities
   model
   o3ds/o3ds
   
Links
=====
* `GitHub <https://github.com/mocap-ca/Open3DStream>`_
* `Discord <https://discord.gg/GndFxeM]>`_
* `Contact <https://open3dstream.com/contact>`_
 

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`