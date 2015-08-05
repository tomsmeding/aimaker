Aimaker Reference (commit 6dc8071)
==================================

Variables
---------

All variables starting with `_` are immutable, which means you can't create
variables whose name starts with an `_`.

| | |
|-----------:|:------------|
|Name|_prevloc|
|Type|number|
|Description|The source location of the previous goto; jumping to `_prevloc` starts execution after that `goto` statement.|
|Example|`goto _prevloc`|

| | |
|-----------:|:------------|
|Name|_rip|
|Type|number|
|Description|The current location in the code pages.<br>(current page and current instruction)|
|Example|`goto _rip`|

Meta attributes
---------------

| | |
|-----------:|:------------|
|Name|name|
|Type|string|
|Description|The name of the program.|
|Example|`#name My cool bot`|

| | |
|-----------:|:------------|
|Name|author|
|Type|string|
|Description|The author of the program.|
|Example|`#author Hacker`|

Functions
---------

All function names are case insensitive.

| | |
|-----------:|:------------|
|Function|move/walk|
|Description|Moves the current bot forwards (1) or backwards (0).|
|Ticks|5|
|Tier|0|

| | |
|-----------:|:------------|
|Function|rotate/rot/turn|
|Description|Rotates the current bot clockwise (1) or counter-clockwise (-1).|
|Ticks|5|
|Tier|0|

| | |
|-----------:|:------------|
|Function|nop|
|Description|Waits a tick.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|goto|
|Arguments|- label<br>- swag|
|Description|Goes to the specified `label`.<br>`goto a` is the same as `if 1, a`.|
|Ticks|1 (+ 2 if trans-page)|
|Tier|0|

| | |
|-----------:|:------------|
|Function|ifgoto/if|
|Arguments|- condition<br>- label|
|Description|Goes to the specified `label` if the `condition` evaluates to a truely value.|
|Ticks|2 (+ 2 if trans-page)|
|Tier|0|

| | |
|-----------:|:------------|
|Function|store/sto|
|Arguments|- variable name<br>- value|
|Description|Stores the given `value` in `variable name`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|getloc/loc|
|Arguments|- x variable name<br>- y variable name|
|Description|Gets the current x and y location and puts the values respectively in the given variables.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|getdir/dir|
|Arguments|- dir variable name|
|Description|Gets the current direction and puts the value in the given variable name.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|page|
|Arguments|- page id|
|Description|Jumps to the page with the given ID.|
|Ticks|2|
|Tier|0|

| | |
|-----------:|:------------|
|Function|transfer/trans|
|Arguments|- page ID<br>- target ID|
|Description|Copies the page at the given `pageId` on the current bot to the given `targetId` on the bot directly in front of the current bot, overwriting everything that was there originally. If there isn't any other bot there, this function does nothing, but doesn't take any shorter.|
|Ticks|`floor(5 + log10(pagesize + 1))`|
|Tier|1|

| | |
|-----------:|:------------|
|Function|transferlocal/translocal|
|Arguments|- page ID<br>- target ID|
|Description|Copies the page at the given `pageId` on the current bot to the given `targetId` on the current bot, overwriting everything that was there originally.|
|Ticks|`floor(3 + log10(pagesize + 1))`|
|Tier|1|

| | |
|-----------:|:------------|
|Function|suicide/fuckit|
|Description|Kill yourself.|
|Ticks|∞|
|Tier|0|

| | |
|-----------:|:------------|
|Function|look|
|Arguments|- var name|
|Description|Looks what's infront of the bot and sets the value to `varName`, each bit of the value that is set to `varName` has a special meaning:<br>- bit 1: is_wall<br>- bit 2: is_bot<br>- bit 4: is_my_bot<br>- bit 8: is_bot_sleeping|
|Ticks|3|
|Tier|0|

| | |
|-----------:|:------------|
|Function|build|
|Arguments|- tier (one of: 0, 1, 2)|
|Description|Builds a bot infront of the current bot, if there's a bot in front of the current bot, this function will do nothing but will take the same amount of time. The higher the given `tier` tier is, the more the created bot can do.  See the `Tier` property of each function in the reference for more information.|
|Ticks|7 + 4 * tier|
|Tier|2|

| | |
|-----------:|:------------|
|Function|wake/shake-awake|
|Description|Wakes the bot in front of you.|
|Ticks|3|
|Tier|1|

| | |
|-----------:|:------------|
|Function|sleep|
|Description|zzz|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|getat/at|
|Arguments|- arrayVarName<br>- index<br>- varName|
|Description|`varName = arrayVarName[index]`|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|makearr/arr|
|Arguments|- varName|
|Description|Creates an array on `varName`, overwriting anything that was stored there already.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|length/len|
|Arguments|- arrayVarName<br>- varName|
|Description|`varName = arrayVarName.size()`|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|push|
|Arguments|- arrayVarName<br>- value|
|Description|Pushes `value` to the array at `arrayVarName`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|del|
|Arguments|- arrayVarName<br>- index|
|Description|Removes the item at `index` on the array at `arrayVarName`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|print|
|Arguments|- thing|
|Description|Prints the given `thing`.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|print-vars|
|Description|Prints all the current bot's stored variables and their value.|
|Ticks|1|
|Tier|0|

Debugging functions
-------------------

Requires you to use the --allowdebug flag.

| | |
|-----------:|:------------|
|Function|stop-match|
|Description|Stops the current match.|
|Ticks|1|
|Tier|0|

| | |
|-----------:|:------------|
|Function|break/breakpoint|
|Description|Breaks execution, resumes after pressing the <enter> key.|
|Ticks|1|
|Tier|0|
