## Automation 

for MQTT Hyperdash V.1.02 (c) by Markus Hoffmann 
(concept studie from 2010)


Based on [rule engines](MQTT-rule-engine-howto.md) an automation concept can
be describes which theoretically leads to a fully autonomous entity of machines, 
all connected to each other via the MQTT framework.

The rule engines allow to crate any level of 
abstraction on the raw data and thus reduce the complexity of the individual
signals as well as maintainig actual/measured states the subsystems are to 
be in. A strong distinction between setpoint- and actual-measured values is 
essential here (actual value vs. target value).
 Both are supported by the 
[MQTT-Hyperdash naming convention](MQTT-dashgen-naming-conventions.md).

In particular, however, they are also suitable for generating status displays
for a higher-level system from a set of other, more subordinate status
parameters: e.g. the state "The power supply is switched on and ready" can be
generated from a series of individual messages from the individual power supply
units and current or power measurements. A well-designed system also allows you
to identify a number of fault conditions yourself. For this purpose, the
hardware usually already provides error bits that can be evaluated accordingly.

A total status for a subsystem or even the entire area can be derived from the
sum reports (with the help of the rule machines). This is an essential task of
the control systems and helps the surgeon to get an overview before having to
deal with all details and individual messages for troubleshooting, for example.

The prerequisite here, however, is that each individual component down to the
hardware level must also generate adequate control signals which permit such a
complete determination of the state. The criteria for the design of (new)
hardware components must be aligned accordingly.

In addition to the pure state detection, there must also be a way to change the
state of a system in a targeted manner (e.g. switching on a previously switched
off power supply or switching on an entire subsystem including its
conditioning).

So, directed rule engines allone cannot map the processes and algorithms to
really automate the  system. Therefor we have to introduce another extended
class of rules, called "Intentions". Intentions can be iplemented as rule
engines with some additional special state parameters. However we want to look
at them from a different perspective at first.

### "Intentions" for process automation

A "complete system" provides the functionality to reach every state in a
targeted manner. This cannot be realized in practice. However, it is
practicable that all relevant operating states can be reached, and at least for
a number of "errors" or "exceptions" which occur outside the normal
functionality, the error state can be left again for a normal operating state.

If this is not guaranteed, the system comes into a state where manual
intervention (if necessary with a screwdriver or programming tool) is
unavoidable. This stands in the way of (complete) automation of the overall
system. For this reason, as much expert knowledge as possible about
troubleshooting, conditioning, operational sequences, etc. must be made
available as a functionality. This happens on the lowest level (near the
hardware) and on all levels above, up to the highest level, where ideally only
one switch chooses between the states "On" and "Off".

In order to achieve this full automation, not only rules must be created that
reliably recognize all states, but also regulations/procedures to achieve
desired states (so-called "intentions"). These are usually instructions to the
subsystems, which occur in a specific order and vary depending on the state of
the system that has already been reached. 

The abundance of instructions for all components of the overall system to
achieve a normal operating state can quickly become confusing and ultimately
very inflexible. It is therefore important to create a system and an
implementation framework in which the automation in a hierarchy can be carried
out as context-locally as possible with individual, clear instructions.

Large procedures are to be factored into smaller sub-steps where possible, then
an automatism can ultimately also find detours in the overall procedure if a
originally planned route (due to e.g. the failure of a subsystem; sudden
change in status at one point) can no longer be committed. Possibly, the
higher-level status can still be achieved in this way, even though there is a
deviation from the standard procedure.

With the appropriate formulation of the individual tasks, a large part of the
compositions can already be done dynamically and therefore automatically. This
goal is to be achieved by the concept presented here.

#### Observables

Observables are system parameters from which a state can be derived using a rule
or a set of rules. This process is also called quantization of the
multidimensional and continuous real state space.

The measured values stored in parameters, e.g. from an ADC card to which
temperature sensors are connected, are observables for the thermal state of a
device. This state can be, depending on the measured temperatures, e.g. "too
cold", "correct", "too warm" or "critically hot".

An observable can also be a state of another system on which that system
depends. Ultimately, observables should be based on measured values and
enable statements to be made about the condition of the device to be checked.

Expressions of desire of the operator, e.g. an entered numerical value, could
also be regarded as an observable (looking at the human as a "device"), but this
is not particularly useful in this concept.


#### States 

A (complete) set of n states is defined for each system (e.g. "on", "off" and
"broken"). For this set, a status parameter (integer) is kept ready in the
control system and a rule machine maps (starting from a series of observables)
to this parameter (see Figure). This ensures that the actual status of the
system - based on the respective status record - is known at all times.

<img src="images/statedetection.png">

Figure: Actual states are derived from observables using a set of rules. The
observables can also include a history which is stored in the memory or is also
available as a parameter in the parameter database.

Determining meaningful states is a non-trivial matter and is usually done by the
person who developed the system or who knows it best. The states can be defined
in different ways, but it must always be ensured that the abstract states
clearly reflect the operating state of the device. A state "undefined" is also a
state in the strictest sense, but hardly anything can be derived from such a
state, in particular nothing that leads to an automatic finding of a state
appropriate to the function (e.g. "ready for operation").

It is also possible to define several sets of states that are complete in
themselves (i.e. represent a partition of the entire state space), e.g. "On" and
"Off", as well as "ready for operation" and "faulty", but where conditions from
one of the records may overlap. Each set of states must be reflected in its own
parameter.

#### State changes and Transitions

A change of state can happen on its own, e.g. by the failure of a system, by
reaching a temperature threshold or by the expiry of a preheating time or by
other physical processes which are detected by the diagnostic system.

Secondly, these physical processes can also be triggered by the diverse control
functions. In this case, an action has resulted in a transition from state A to
state B. Each action, on the other hand, can in turn be given by an intention,
whereby we want to understand intention as the desired target state combined
with a procedure that should lead to that target state from theoretical
understanding (and hopefully also practically, if the system works as intended).
However, whether the condition is reached is not guaranteed (however, this
should be the case if the system functions normally). This can be verified or
falsified by observing the detected actual state, which correspond to the intendet state.
If they are different, action need to be taken.


<img src="images/transitiontable.png">

Figure: Transition matrix from one state (initial) to another (final) and the
various possible and impossible actions. The path to a desired final state can
be found automatically.


The problem of automation can now be summed up quite simply, independently of
the complexity of the system and regardless of the number of subsystems
involved:

<pre>

The following applies to all systems: 
If the system is not in the desired state, 
it will do nothing more than (permanently) by itself 
try to get into this state.

</pre>

The system automatically waits for a (possibly system-external) condition to
occur and then continues. If the state can be reached automatically, it will be.
Otherwise, manual intervention is usually required (with a screwdriver). Then
either the hardware is physically broken and has to be repaired or replaced, or
there is no automatic procedure for this fault situation that can bypass or
remedy the fault. This procedure would then have to be retrofitted.

No other case can occur in a system for which all relevant observables have been
provided. So it should not happen that operating the system (manually, but with
the functionality provided) only continues if external information (e.g.
evaluating an oscillograph image) is used to make a decision. If the latter is
the case with a system, the design of the system in question has to be rethought
and a new measurement signal may have to be installed so that automatic operation
(only then) becomes possible.

All in all, this process of continuous improvement, if resolved consistently,
should ultimately lead to complete automation, with all of the knowledge about
the operation of the individual components gradually being incorporated into the
control system software framework (from which it can then be extracted, for
example, for documentation purposes).

What is important is a framework and a language in which the implementation can
be carried out as easily as possible. As many people as possible should be able
to contribute. A large part of the process should already take place
automatically in a standardized way according to defined rules. The knowledge
then only has to be poured into simple rules that only describe formalized
transitions or actions. In their overall direction, these automatically allow
the automation of the entire system. The detailed knowledge from the lower level
subsystems  should not be needed for the higher hierarchical
level.


Rules and intentions are closely related. In fact, an action associated with an
intention is intended to reverse the rule machine graph. If a rule is a pure
conversion formula, an inverse function can easily be defined if necessary. Most
of the rules associated with state detection are only surjective (quantization)
and are no longer injective. A clear reversal function cannot therefore be
specified. However, control and regulation tasks can usually still be solved by
iteration, in which case an (optimal) initial state is selected and found from
all possible ones that lead to the same final state.

If the directed graph of the rules projects upwards from the observables to a
state, the chained intentions represent a reversal of the direction from top to
bottom. This direction is usually more difficult to achieve, and the rules for
this are also more complicated, since they must contain knowledge of how it
works and an expectation of the likely behavior of the system (usually from a
model or a (physical) theory). However, since these models can only access
quantized information that is disturbed by the measurement accuracy, they have
to make certain assumptions that are not guaranteed to, but probably must, apply.
There are therefore "good" and "bad" procedures, in the sense that the former
are more likely to achieve the desired goal. This is the reason for the
particular difficulty in automation.


#### Implementing Intentions

Following belongs to an "intention":

1. two corresponding parameters:
   a) the detected state (actual state) and corresponding
   b) the desired state (target state),
2. a representation of the transition matrix with entries about internal actions, combinations of intentions and prohibited transitions,
3. a table with heuristic evaluation factors ("lengths"),
4. optionally one or more rules and
5. optionally one or more internal promotions with associated ratings.

<img src="images/intention.png">
Figure: How an intention works in the automation framework.

A rule that contains the two parameters as trigger parameters in its input
quantity (and optionally any number of others) is triggered whenever either the
current detected state changes or the target state changes. The rule selects a
transition from the set of the maximum n*(n-1) possible transitions which are
arranged in a transition matrix. This is based on another rule for evaluating
the cheapest or shortest of all possible paths. These paths consist of a chain of
transitions that should lead to the target state via any detour states. The
first of these actions is then triggered and should now bring the system into a
state that is closer to the target. Then the rule is triggered again and the
next step is carried out until the goal is reached, in which case nothing is
done. If the target cannot be reached by this way, something is fundamentally
wrong and need to be (manually) ficed. This can be easily detected by a 
permanent discrepancy between the actual and current state parameters which 
could flag a warning.

An action can either be carried out within the system, e.g. if the system is
directly connected to the hardware, or it is defined by a number of other
intentions that affect other (subordinate) systems. In the first case, the
action excecutes a procedure that does something locally (i.e. on
the computer where it runs, interfacing the connected hardware).

In the latter case, it is sufficient to trigger certain other intentions
(whereby the order should not matter, since the intentions are expressed almost
simultaneously). (Sequences have to be implemented in a different way, see
a chapter below.) 

#### Internal Actions

Every action, or each path of actions, which is followed by intentions, can be
broken down into more and more finer actions, which at the bottom end always
result in internal actions. These are then ultimately carried out by the servers
of the hardware devices. The individual actions take place autonomously and, if
necessary, simultaneously on each server/device.  In order to find favorable
ways out of the considerable amount of the different possible paths of a
transition, a criterion has to be used,
which takes into account which of the routes is the shortest and is accordingly 
preferred if it is not blocked.

Suppose a system that is in the "ON" state is to change to the "OFF" state.
Assume there are two ways to do this. First, it can change directly to the off
state, and secondly it can go to the standby state and then to the off state.
The last route is obviously longer and therefore the direct route should be
followed. 

Or another example: Should several systems intentionally go to other
states, the evaluation of this transition depends on how many systems are
already in the desired state.
Thus, for the transition of an exemplary system from the "not ready" state to 
the "all ready" state, all 300 subsystems must be in the "ready" state. 
However, the "length" of this route is certainly dependent on the number of 
subsystems that are already in the desired state.

Rules must therefore be found to evaluate these actions. The following approach
is suggested here:


1. Internal actions get a heuristically found evaluation factor ("length"),
whereby a length of 0 means that the action does not need to be carried out 
because the state has already been reached.

A length of 1 means a normal step, e.g. a switching process with a duration of
100 ms, or an action that leads to resource consumption (wear and tear due to
switching or similar), in the sense of, switching may occur approx. 1 time per
minute, without reducing the life of the part to less than the total life of the
overall system.

A length less than 1 means the step is shorter or faster than a "normal step".

A length greater than 1 means that the action is more expensive, takes longer or
cannot be carried out as often because it consumes more resources.

An infinite length means that the action is prohibited and may not or must not
be carried out. In practice, a high maximum value is used instead of infinity,
e.g. 65000. Lengths greater than or equal to this value are then considered to
be infinite. Length calculations that reach or exceed this value are canceled.

2. The length of actions to be carried out in parallel is calculated according to:


l = w * \sum_i l_i

where l_i are the lengths of the individual actions and w is a positive weight 
factor w>0, which takes into account the fact that the lengths of an overall 
action need not necessarily be the sum of the lengths of the individual 
actions, e.g. setting multiple bits of the same hardware cannot consume 
additional resources because it happens simultaneously or because the entire 
register is always set anyway. In this case w = 1 / n.

Individual intentions that do not lead to any action because the state has
already been reached do not make any contribution, in this case the length
calculation for the individual system will deliver l_i = 0. If a prohibited
action is involved, the overall action is also prohibited, namely if w>=1.

In this way, "lengths" (recursive) can be calculated for all transitions.

The path with the smallest overall length is then the cheapest. So the intention
triggers a transition, which represents a step in this direction.

#### Mixed Actions

Not all intentions have exclusively internal actions or only external actions.
The possible actions can also be mixed. For example, the Magnetic
field of a magnet can only reach the "normal" state if 
a) the power supply is in the "On" state and additionally 
b) the setpoint is set to a certain value. 
The action which is to establish this state will therefore send an external
intention "on" to the power supply unit and in addition set the e.g. current
parameter "current setpoint" (in relation to the intention "on") to a certain
value, which counts as internal Action (of the intention "normal"). Note
that the order of the two individual actions is not important.

The length of this action is then calculated from the length of the intention
"On" plus one internal length for setting the setpoint.

Caution: Changing non-free parameters can trigger uncontrolled changes in status
within other intentions. In this case, this action is not really internal.

Therefore, this must be avoided (through careful planning) so that there are no
contradictions in the rules and the automation becomes unstable.

Definition: "Elementary intentions" are those that are only defined through
internal actions. Here the distance matrix is fixed and does not need to be
calculated dynamically.

Definition: "Sufficient intentions" are those that are only defined through
external actions.

Here the whole set of rules can be generated automatically. This includes:
condition detection, distance matrix calculation and automatic path 
finding of the intention.




<img src="images/intention2.png">

Illustration: Intention with autonomous path length calculation: Rule R2 always
knows the path lengths from the current state to all possible other states that
can be reached. In the form of a distance matrix, these are also made known to
all other intentions as parameters. These in turn recalculate their lengths accordingly.
Rule R1 is used to determine the current status and rule R3 monitors changes in
the target state and in the actual state and if necessary, apply a step from the
shortest path heading the target state.





### Intentions with autonomous path length calculations

(Why it is better not to use sequences).


The implementation of procedures in so-called sequences, i.e. chronologically
ordered instructions, which are processed sequentially (i.e. one after the
other), seems to be more suitable and easier to implement at least for some
tasks than the definition of many extra states with corresponding dependencies.

A well designed implementation of a sequence must always take into account the 
risk that certain instructions from the sequence are not executed correctly. 
After each step in the
sequence, you should actually first carefully check whether the desired action
was carried out without errors. If not, the sequence is usually not allowed to
continue and would either have to be terminated or to run in one of a number of
branches that take account of the error that occurred and, if possible, return
to the actual sequence. It is hardly possible to catch all possible errors in
this way, so the sequence will very likely miss an error, and the machine will
not end up in the achieved state, but instead in an uncontrolled other state
(because, for example, the sequence simply continued even though an error
occurred in a substep that was overlooked). In order to find out in which one, a
complex error analysis procedure is required.

In short: For small and reliable steps, a sequence can make sense in terms of 
simplicity, clarity and quick implementation.

For example, in the way they can appear in the definition
of rules and intentions. In the case of the rules,
however, they should mainly be used to implement
algorithms, i.e. pure computing processes, where only one
step should be calculated in iterative processes.
Sequences in rule definitions should therefore not be used
to query and set parameters outside those agreed in the
set of input and output parameters, although this is not
explicitly prohibited, and in some cases can make sense.
For intentions, the consideration only applies to the
"internal" actions anyway, since by definition all others
cannot be sequences, but may trigger additional rules and
intentions. In the case of the internal actions, it must
therefore also be ensured that a possible success or
failure of the sequence can later be detected on the basis
of suitable measured states. In this case the internal
actions can be seen as "fire and forget" sequences.


Relatively quickly, however, a sequence becomes susceptible to incomplete
implementation and the resulting uncontrolled changes in state, which, after a
certain level of complexity (which is reached quite quickly), becomes uncontrollable
and therefore unreliable. This is contrary to the desired robustness.

This concept is therefore intended to limit the use of sequences to an absolutely
necessary extent and only allow them where the instructions can be safely executed
or where there is (yet) no diagnosis for the detection of errors anyway.

In all other cases, the use of sequences (i.e. actions that cannot be triggered at
the same time where the order is important) should therefore be avoided.

A consistent implementation using the intentions and rules automatically checks the
statuses achieved and finds its way independently. The sequence of actions to be
processed one after the other is realized by the dependencies of the states, which
ensures that an action is only triggered when the target state of the preceding
action has actually been reached. Otherwise, another action is automatically
triggered, which tries to correct the previous error and then continues normally in
the chain of actions. The sequence then arises from the continuous path of the
states.

### Distinguishing: free and non-free states and parameters

Definition: Free parameters are those that are not in any rule for determining a
state that is part of an intention.

Rules whose inputs only consist of free parameters produce free parameters as
outputs. User inputs can also be free parameters, e.g. a temperature setpoint.

Definition: Free states are those that are not part of an intention. A free state is
represented by a free (integer) parameter. You cannot get to a free state in a
targeted manner. It is only suitable for diagnosis.

### Avoiding contradictions in the rule machinery

An interesting feature of the automation concept presented here is that
contradictions automatically prohibit themselves. That is, states that are involved
in such contradictions cannot be reached automatically.

However, this does not mean that no contradictions can be constructed. For example,
two rules can form a cycle that causes the parameters involved to oscillate and thus
show unstable behavior. Great care must therefore always be taken when cycles are
used in the regulations.

Contradictions in intentions can also be expressed in another form: e.g. an action
of another intention can demand a state, which in turn leads to the former intention
changing, because in turn a different state is required by the former.

This can also form cycles. Cycles of this type, however, are already noticeable in
the autonomous length calculation and lead to the lengths of the paths concerned
becoming longer and longer and diverging in an iterative step. Eventually a maximum
length is reached where the process stops. However, maximum length means that this
path becomes a forbidden path. In this way, it is possible that all paths that lead
to conflicting states are prohibited. These problems are then exposed through a
permanent discrepancy between target and actual states of some intentions, where the
problem can then be easily localized and hopefully remedied.

### Consequences

From the described automation concept there are some (quite desirable) implications
which should be considered:

1. Non-converging (unstable) cycles in the set of rules eliminate themselves since
their length adds up to infinity, and these paths are then prohibited.

2. Distance computing will consume a great deal of computing power, since every 
   change in the state of the subordinate systems, if it leads to a change in 
   length there, triggers the recalculation of the distance matrices of all 
   superordinate systems.
   But since everything happens in parallel, the load for a single computer is low.


### A path finder

The shortest path is to be found from a series of possible paths from an initial
state A to a final state B.

Given a transition matrix with weights (or lengths), standard algorithms from
graph theory can be used. For example, Dijkstra's algorithm. Our problem presents
itself as an edge-weighted graph, in which the edge weights have to be calculated
according to the actual state and possibly recursively from the states and actions
of the external intentions involved or through the autonomous path length
calculation.

Dijkstra's algorithm (after its inventor Edsger W. Dijkstra) is used to calculate a
shortest path between a start node and any node in an edge-weighted graph. The
weights must not be negative.

For non-contiguous, undirected graphs, the distance to certain nodes can also be
infinite if a path between the start node and this node does not exist.

The same applies to directed, not strongly connected graphs. These requirements
apply to our problem.

The algorithm works as follows: The next best node with the shortest path is
successively included in a result set and removed from the set of nodes still to be
processed.

Route planners are a prominent example where this algorithm can be used. The graph 
here represents the road network that connects different points. We are looking 
for the shortest route between two points. Dijkstra's algorithm is
also used on the Internet as a routing algorithm in OSPF (Open Shortest Path First, 
is a term used in computer network technology).

An alternative algorithm for finding the shortest paths, which is based on 
Bellman's principle of optimality, is the Floyd Warshall algorithm. 
The principle of optimality states that if the shortest path leads from A to C 
via B, the partial path A B must also be the shortest path from A to B.
   
