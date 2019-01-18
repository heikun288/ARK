# Directory structure

> [!NOTE]
> ARK directory sturcture

```
|-- Bin                         #binary dir
|   |-- bin                     #excutable files dir
|   |-- binlog                  #log dir
|   |-- bus_conf                #bus config dir
|   |-- lib                     #plugin and 3rd dynamic libraries dir
|   |-- plugin_conf             #plugin config dir
|   |-- resource                #designer
|   |   |-- client              #csv files for client
|   |   |-- excel               #excel files for designer
|   |   |-- mysql               #mysql schema for db
|   |   |-- proto               #name file that be generated by tools
|   |   |-- res                 #xml files that be generated from excel files by tools
|   |   |-- schema              #schema files thet be generated from excel files by tools
|   |   |-- script              #lua script(for now, not used)
|   |-- tools                   #transfer excels to xmls
|-- cmake                       #3rd cmake dependencies
|-- Dep                         #3rd libraries
|   |-- args                    #parse command line arguments
|   |-- common                  #some OS mini hpp files
|   |-- rapidxml                #operate xml files
|   |-- spdlog                  #log library
|-- Frame                       #source code
|   |-- Common                  #some common header files
|   |-- Examples                #examples
|   |-- Launcher                #the executor which launch plugins
|   |-- SDK                     #general plugins and base libs
|   |   |-- BusPlugin           #bus plugin
|   |   |-- Core                #core library, supply some base structures
|   |   |-- Interface           #SDK interfaces
|   |   |-- KernelPlugin        #Kernel plugin, include Entity management and data management, very important
|   |   |-- NetPlugin           #network plugin, supply TCP http websock server and client
|   |   |-- Proto               #self protocol files
|   |   |   |-- cpp
|   |   |   |-- cs
|   |   |   |-- js
|   |   |   |-- proto
|   |   |   |-- proto-gen
|   |   |-- UtilityPlugin       #some useful module, like guid, log, etc.
|   |-- Server                  #server plugins
|   |   |-- DBPlugin            #database proxy plugin
|   |   |-- DirPlugin           #directory plugin
|   |   |-- GamePlugin          #game plugin, main game logic
|   |   |-- Interface           #server interfaces
|   |   |-- LoginPlugin         #login plugin
|   |   |-- LogPlugin           #remote log plugin
|   |   |-- MasterPlugin        #master plugin, manage all other processes
|   |   |-- OSSPlugin           #operating support system plugin, just for game operators
|   |   |-- ProxyPlugin         #net traffic bridge for client and server-side
|   |   |-- RouterPlugin        #transfer net traffic to right processes
|   |   |-- WorldPlugin         #world plugin, manage zone processes, like game, proxy, login, etc.
|   |-- Tools                   #tools for this project
|       |-- Conv2UTF8           #transfer files to UTF8
|       |-- FileProcessor       #transfer excel files to xml
|-- Plugins                     #3rd plugins, like mysql, redis, etc.
`-- Test                        #some test files(for now, not used)
```