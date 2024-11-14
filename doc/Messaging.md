# Messaging model

There are two main ways (so far) to pass messages between tasks and have them react.

- Channels a la CSP where two tasks synchronize on a channel and exchange something once synced.  Channels limit the interaction of tasks, and decouple the tasks so that each endpoint does not know the identity of the other.  Also, a task can delegate its endpoint to a child task without the other end needing to know about it.

- Active objects a la actor model where a task receives messages in an input queue and responds based on the message.  Senders may block if the receiver queue is full.  Any task can send a message to any recipient, and recipients must know what to do with each message they receive.

    - Alternatively a task defines the messages it is willing to accept, and a task sending the wrong message hangs because the recipient was unwilling to accept the message.  This could also be some kind of fault condition that triggers an error back to the sender.

    - Delegation is ugly here, because the sending task determines where the message goes, rather than the recipient.

Determination
: Use channels for message passing.

# Channel structure

In order to decouple sender and receiver (reader and writer), channels need to provide storage space for the data exchange.  No they don't.  A channel only needs to identify the task that is waiting for either the read or write.  But the channel does need to identify the reading and writing task IDs so that they can find each other.  When a task delegates its end of a channel to a child task, then it must update the channel with the child task ID rather than its own.

At the assembly level, this is notionally a 'push' of the channel to a new task ID, so that a task can give up a channel to another task, and thus the channel is always associated with one or two tasks.  When the channel is first allocated, a single task has both ends, and then from there it can push one of the ends (or both) to other tasks, as would be seen in INC going into a parallel.

# Alternation

One of the trickier bits so far is a good system to perform a 'select' to wait for one of any number of channels to become ready before committing to a read or write.  Some proposals.

- A task can ascertain the sync status of a channel in a nonblocking manner, sleeping and looping if none are available.  Two alt-ing tasks would be unable to find each other in this system.

- A 'matchmaker' task in the system looks for tasks that are waiting for the same channel ID and pairs them together into a commitment.  The data structures for this aren't worked out.  Also, there may be some nonlinear time complexity to worry about, or a large space tradeoff otherwise.

- A multilevel process of partial commitments to a channel, such that a task can inform other tasks via the channel that it is alt-ing, and so any other task that wants to commit to it must return control to the alting task.  When both tasks are alting, there is another layer of the protocol to offer an exclusive commitment, so that one alting task cannot trick the other one into hanging.

# Alt Instructions

Here is a possible set of alt instructions assuming tasks can negotiate an alt protocol directly without the system.

ALTSTART
: Set counter to 0
: Set 'alting' flag that prevents interrupt and limits what other instructions are possible
: Clear 'nowait' flag

ALTREADYREAD
: Increment the counter
: Set reader flag in channel to alt
: Assert this task is the reader in the channel
: Set 'nowait' flag if the other side is ready (or alt-ready, I think)

ALTREADYWRITE
: Same as ALTREADYREAD, but as a writer

ALTWAIT
: Yield out of task queue if 'nowait' flag is clear
: Clear the alt jump address

ALTCHECKREAD
: Decrement counter
: Assert this task is alt ready in channel
: Clear alt ready in channel
: If the other end is a ready writer and the alt jump address is not set yet, set the alt jump address (if the other side is alt ready, this counts)

ALTCHECKWRITE
: Same as ALTCHECKREAD, but as a writer

ALTEND
: Assert counter is 0
: If alt jump address is set, jump
: Clear 'alting' flag to enable interruption and the rest of the instructions

TRYREAD
: Assert this task is reader of channel
: If other side is normal ready writer, do normal in-place read against other task and reset the channel (includes waking other task and moving its PC)
: If other side is alting ready writer, mark the channel as ready to read and yield, but remain in the task queue in case the other task doesn't choose this one
: Needs jump addresses to indicate succcess and failure?  Or to return to the ALTSTART to try again.  Need the state machine of all the possible behaviors a task can have if it wakes up to a TRYREAD instruction

TRYWRITE
: Same as TRYREAD, but for writer

The general idea is for alt to be 2 phases.  In the first, the alting task 'puts itself out there' and solicits interest from other tasks.  If it finds a task already committed, it immediately works with that task.  If it finds another alting task, it immediately tries to work with that task.  If it finds no tasks, it sleeps until one of the channels gets some action on it, which wakes this task back up.  Once awakened, it looks at all the channels for the one that is ready.  It sets up a jump to the first one it finds.  (Not finding one is an error, but jump back to the start of the alt and try again.)  The start/end construct with the counter is to ensure that an alting task cleans up after itself and doesn't leave channels hanging around.

The second phase uses a trial method to sync on a channel if the other side is an alt, so that an alt task is never permanently stuck if it attempts to use an alt channel and the other side reneges on it.

- Consider what happens if a task dies while in the midst of an alt.  This shouldn't break anything, since direct read/write always hang anyway, and another alt task should be able to recover when it wakes up with no channel movement.
