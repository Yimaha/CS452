# Sensor reading, how much can we afford to delay so interrupt tends to be disabled?
## How does the delay work on the sensor level?
The problem I see is that marklin box is taking its sweet fucking time to process sensor query, each sensor query is really lame because it defaultly takes 70ms, and if somehow you send more command in the mean time, you pretty much cuck yourself further.

As an experiment, I tested delay of 100 ms and 70 ms between sensor read, the result is very interesting:
1. If I use 70ms, all the sensor read is returning as fine as long as I don't put any input, yet, if I do something like train command tr, it actually causes delay in read. this means pumping any train command will delay marklin box's processing speed significantly, by matters of 10ms probably.
2. if I do the 100ms, it almost never come back to me late, meaning it can accept 1 train command each sensor query, no problem.
3. this got me curious, what is our limitation of delay?

With this curiosity, I tested with 80ms, and 90ms, to my surprise, 80/90 both work pretty well, allow command pass through yet doesn't limit the actual parsing to enable interrupt.

Thus, propose a first iteration of design: each of the command request by the train server is going to be delayed by 80ms by default, since each 80 ms pulling loop checks the sensor status and able to fire a command anytime in between.

We need a communcation server the ensure things are only fired once every 80 ms from the train server, if not, buffer it. to sync it with the actual parsing of the sensor server, I perfer a way where sensor server tells train_server that it has started a new cycle, thus introducing a new courier.

# Clock::Delay should be banned throughout the duration of train control (terminal is fine)
the main server should be responsible for keeping track of timing issues instead of just letting the sub executor to delay say 7 ticks, because you risk loosing a tick and potentially permanently lose sync of command, I would perfer if main server keep track of ticks increment and use delayUntil to keep track of the changes.

# Union is must for request body
request body must be a union type not struct in the future, since it heavily limit expandability if not and is a pain in the ass to refrac.

# Command queue
due to earlier comment, we need a way to sync command send and provide command parking lot, this generate 1 new api and changes how send works in train server
1. one of the (non-blocking) api should instantly tell the caller when is the next available spot for command, however, you potentially run into the issue of barging and stuff. so this doesn't work
2. in order to avoid this, any sort of command such as rev, speed, sw should obeys some sort of rule
- if train want to change speed, it does care about when the speed command can realistically fire. this means it has to reserve a spot to fire train command. in the non-blocking situation, command fires right away, if not, command can only fire after a while
- if you are a rev command, you are only fired at the beginning (for now), which means you can simply wait for train server tell you reverse is completed, this is low priority
- if you are a swtich command, at the moment right now we don't have on demand switching, meaning that we only swtich at the beginning of the call, thus will not be a problem when it comes to barging (also only 1 train), but suppose you have multiple trains, and provided on demand swtiching, you might need a more precise timing tracker.

This is hard to do, in order to avoid barging in a more naive approach (simpler, but perhaps a bit shittier in terms of pathing, but tbh it might not be that big of an issue). we elevate the issue to the pathing server, this means 
- There is server we manage called global pathing which ensures that both collision doesn't happen and command firing will not collide on the time intervel.
- There smaller localized pathing server submit their plan to global server, and global server decide when to change speed, flip swtich such that collision doesn't happen and such path can be achieved.

This way, we don't need to keep a queue in train server, and what train server simply does is check if you are clear to send, if not, crash the service. (because there must be a bug in the global pathing server)


# Elevation of the delay timing with train server
To give global server a more finegram control, we need a way to let global server in direct control of timing, thus we elevate the control status to the global server. For example, exactly how long does a train stop after reversing command is sent etc is managed by global pathing server, not the train server. Thus, on top of hte existing api, we need stuff like TRAIN_SPEED_RAW, TRAIN_REV_RAW, TRAIN_SWTICH_RAW which straight up check if you can send the byte and send the byte, where the actual timing is in control of the global pathing server.

# Command line control vs Global Pathing control
ideally, when global pathing is running, it should just ban the command from firing cause firing regular command actually fuck up your sync between the sensor server and the train server. (because you are barging in bytes in between train control, yet global pathing expect itself to be the only person generating the command). This is a problem, so when ever you sent tr, sw, or rev, command, it instantly terminate the current path finding algorithm on global scale and stop all trains.

# shifting train logic to global control
For the same reason, train control variable is moved up toward global_pathing_server. what remains in train server is just a simple struct that keep track of train state so terminal can read and report them.
