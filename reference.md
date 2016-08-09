Aimaker Reference (commit a3064d9)
==================================

Variables
---------

All variables starting with `_` are immutable, which means you can't create
variables whose name starts with an `_`.

| | |
|-----------:|:------------|
|Name|<div id="_prevloc">_prevloc</div>|
|Type|number|
|Description|The source location of the previous goto; jumping to `_prevloc` starts execution after that `goto` statement.|
|Example|`goto _prevloc`|

| | |
|-----------:|:------------|
|Name|<div id="_rip">_rip</div>|
|Type|number|
|Description|The current location in the code pages.<br>(current page and current instruction)|
|Example|`goto _rip`|

Every variable that doesn't exist evaluates to 'nil', this means that it's
legal to insert an nonexistent variable into an array; 'nil' will be inserted.

Meta attributes
---------------

| | |
|-----------:|:------------|
|Name|<div id="name">name</div>|
|Type|string|
|Description|The name of the program.|
|Example|`#name My cool bot`|

| | |
|-----------:|:------------|
|Name|<div id="author">author</div>|
|Type|string|
|Description|The author of the program.|
|Example|`#author Hacker`|

Functions
---------

All function names are case insensitive.

| | |
|-----------:|:------------|
|Function|<div id="move">move/walk</div>|
|Description|Moves the current bot forwards (1) or backwards (0).|
|Ticks|If moving forewards: 3, otherwise 4|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="rotate">rotate/rot/turn</div>|
|Description|Rotates the current bot clockwise (1) or counter-clockwise (-1).|
|Ticks|5|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="nop">nop</div>|
|Description|Waits a tick.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="goto">goto</div>|
|Arguments|- label|
|Description|Goes to the specified `label`.<br>`goto a` is the same as `if 1, a`, but faster.|
|Ticks|1 (+ 2 if trans-page)|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="ifgoto">ifgoto/if</div>|
|Arguments|- condition<br>- label|
|Description|Goes to the specified `label` if the `condition` evaluates to a truely value.|
|Ticks|2 (+ 2 if trans-page)|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="store">store/sto</div>|
|Arguments|- variable name<br>- value|
|Description|Stores the given `value` in `variable name`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="getloc">getloc/loc</div>|
|Arguments|- x variable name<br>- y variable name|
|Description|Gets the current x and y location and puts the values respectively in the given variables.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="getdir">getdir/dir</div>|
|Arguments|- dir variable name|
|Description|Gets the current direction and puts the value in the given variable name.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="page">page</div>|
|Arguments|- page id|
|Description|Jumps to the page with the given ID.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="transfer">transfer/trans</div>|
|Arguments|- page ID<br>- target ID|
|Description|Copies the page at the given `pageId` on the current bot to the given `targetId` on the bot directly in front of the current bot, overwriting everything that was there originally. If there isn't any other bot there, this function does nothing, but doesn't take any shorter.|
|Ticks|`floor(5 + log10(pagesize + 1))`|
|Tier|1|

| | |
|-----------:|:------------|
|Function|<div id="transferlocal">transferlocal/translocal</div>|
|Arguments|- page ID<br>- target ID|
|Description|Copies the page at the given `pageId` on the current bot to the given `targetId` on the current bot, overwriting everything that was there originally.|
|Ticks|`floor(3 + log10(pagesize + 1))`|
|Tier|1|

| | |
|-----------:|:------------|
|Function|<div id="suicide">suicide/fuckit</div>|
|Description|Kill yourself.|
|Ticks|∞|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="look">look</div>|
|Arguments|- varName|
|Description|Looks what's infront of the bot and sets the value to `varName`, each bit of the value that is set to `varName` has a special meaning:<br>- bit 1: is_wall<br>- bit 2: is_bot<br>- bit 4: is_my_bot<br>- bit 8: is_bot_sleeping|
|Ticks|3|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="build">build</div>|
|Arguments|- tier (one of: 0, 1, 2)|
|Description|Builds a bot infront of the current bot, if there's a bot in front of the current bot, this function will do nothing but will take the same amount of time. The higher the given `tier` tier is, the more the created bot can do.  See the `Tier` property of each function in the reference for more information.|
|Ticks|`floor(20 + pow(7, arg))`|
|Tier|2|

| | |
|-----------:|:------------|
|Function|<div id="wake">wake/shake-awake</div>|
|Description|Wakes the bot in front of you.|
|Ticks|4|
|Tier|1|

| | |
|-----------:|:------------|
|Function|<div id="sleep">sleep</div>|
|Description|zzz|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="getat">getat/at</div>|
|Arguments|- arrayVarName<br>- index<br>- varName|
|Description|`varName = arrayVarName[index]`|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="makearr">makearr/arr</div>|
|Arguments|- varName|
|Description|Creates an array on `varName`, overwriting anything that was stored there already.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="length">length/len</div>|
|Arguments|- arrayVarName<br>- varName|
|Description|`varName = arrayVarName.size()`|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="push">push</div>|
|Arguments|- arrayVarName<br>- value|
|Description|Pushes `value` to the array at `arrayVarName`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="del">del</div>|
|Arguments|- arrayVarName<br>- index|
|Description|Removes the item at `index` on the array at `arrayVarName`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="print">print</div>|
|Arguments|- thing|
|Description|Prints the given `thing`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="print-vars">print-vars</div>|
|Description|Prints all the current bot's stored variables and their value.|
|Ticks|1|
|Tier|0|

Debugging functions
-------------------

Requires you to use the `--allowdebug` flag.

| | |
|-----------:|:------------|
|Function|<div id="stop-match">stop-match</div>|
|Description|Stops the current match.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|<div id="break">break/breakpoint</div>|
|Description|Breaks execution, resumes after pressing the <enter> key.|
|Ticks|1|
|Tier|0|