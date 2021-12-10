
see https://github.com/jek/blinker https://github.com/jek/blinker/blob/master/tests/_test_async.py

```python
>>> from blinker import signal
>>> started = signal('round-started')
>>> def each(round):
...     print "Round %s!" % round
...
>>> started.connect(each)

>>> def round_two(round):
...     print "This is round two."
...
>>> started.connect(round_two, sender=2)

>>> for round in range(1, 4):
...     started.send(round)
...
Round 1!
Round 2!
This is round two.
Round 3!
```

see https://github.com/pyeve/events

```python
>>> def something_changed(reason):
...     print "something changed because %s" % reason

>>> from events import Events
>>> events = Events()
>>> events.on_change += something_changed
>>> events.on_change('it had to happen')
'something changed because it had to happen'
```
