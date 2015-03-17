# HANDLED XPATH EXPRESSIONS #

### Relative xpath: ###
  * _"n0"_

### Absolute xpath: ###
  * _"/n0"_

### Special nodes: ###
  * _"`*`"_
  * _"node()"_
  * _"text()"_
  * _"comment()"_
  * _"processing-instruction()"_

### Conditions: ###
  * Attribute string value: _"/n0`[`@a=value]"_
  * Attribute int value: _"/n0`[`@a=value]"_
  * Attribute int value: _"/n0`[`@a!=value]"_
  * Attribute int value: _"/n0`[`@a<value]"_
  * Attribute int value: _"/n0`[`@a>value]"_
  * Attribute int value: _"/n0`[`@a<=value]"_
  * Attribute int value: _"/n0`[`@a>=value]"_
  * Node position: _"/n0`[`first()]"_
  * Node position: _"/n0`[`first()+2]"_
  * Node position: _"/n0`[`last()]"_
  * Node position: _"/n0`[`last()-3]"_
  * Node position: _"/n0`[`position() = 0]"_
  * Node position: _"/n0`[`position() != 0]"_
  * Node position: _"/n0`[`position() > 1]"_
  * Node position: _"/n0`[`position() < 2]"_
  * Node position: _"/n0`[`position() >= 1]"_
  * Node position: _"/n0`[`position() <= 2]"_
  * Node position: _"/n0`[`2]"_
  * Other xpath: _"/n0`[`n1/@attr]"_

### Shorten xpath for: ###
  * Children: _"/n0/n1/n2"_
  * Attributes: _"/n0/n1/@a"_
  * Descendent or self::node(): _"/n0//n5"_
  * Parent: _"/n0/n1/../n1"_
  * Self: _"/n0/n1/."_
  * Nodes in namespaces : _"/test::n0"_

### Full xpath for: ###
  * Nodes: _"/n0/n1/child::a"_
  * Attributes: _"/n0/n1/attribute::a"_
  * Descendent or self: _"/n0/descendant-or-self::n5"_
  * Parent: _"/child::n0/child::n1/parent::/n1"_
  * Self: _"/child::n0/child::n1/self::"_
  * Preceding: _"preceding::n1"_
  * Following: _"following::n1"_
  * Ancestor: _"ancestor::n2"_
  * Ancestor-or-self: _"ancestor-or-self::n2"_
  * Namespaces: _"namespace::test"_