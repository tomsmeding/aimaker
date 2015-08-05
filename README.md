Aimaker
=======

A robot competition suite with its own language.

To (re)compile everything:
```
make remake
```

Then, to watch a game between `kaas.bot` and `spam.bot`:
```
./main kaas.bot spam.bot
```

To generate the text reference:
```
docs/gendocs.js --type text
```

Possible `--type` options currently are: `text` and `markdown`. You can create your
own type options by making an docs generator in `docs/generators`, see
`docs/generators/text/` for an example.
