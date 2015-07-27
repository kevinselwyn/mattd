#mattd

Matt Daemon is a Matt Damon daemon

##Installation

```bash
make && sudo make install
```

##Usage

```
Usage: mattd [start|stop|restart|status]
```

##Explanation

Matt Daemon is a daemon that can't be stopped. Unless you sudo.

```
$ mattd start

Starting Matt Daemon... [ OK ]

$ mattd status

Matt Daemon is running.

$ mattd stop

You can't stop Matt Daemon. Permission denied. [ FAILED ]

$ sudo mattd stop

Stopping Matt Daemon... [ OK ]
```

Note: This version does not use `service` like in the example below because Mac.

##Acknowledgements

Inspired by [this blog post](http://blog.yjl.im/2012/02/you-cant-stop-matt-daemon.html) and the actor.

![Matt Damon](https://i.imgur.com/vRRMsnD.gif)