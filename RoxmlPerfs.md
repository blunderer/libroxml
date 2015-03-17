# Introduction #

This page show the libroxml performances.

Tests are made on a Intel Core 2 duo with 4Go RAM running debian etch.

test xml files are generated using xmlgen from http://xml-benchmark.org

libxml2 performances are used as reference, but libroxml does not really compare to libxml2 since it does implement only a few of the XML specification.

# Memory Performances #

### library size ###
| **libroxml** | **libxml2** |
|:-------------|:------------|
| 45K | 1,3M |

### Load/Parse time ###
| **File** | **Node number** | **libroxml** | **libxml2** |
|:---------|:----------------|:-------------|:------------|
| test-27K.xml | 382 | 0.000684 sec | 0.000581 sec |
| test-1200K.xml | 17131 | 0.030720 sec | 0.022027 sec |
| test-12M.xml | 167864 | 0.420765 sec | 0.210274 sec |
| test-112M.xml | 1666310 | 3.013682 sec | 2.088026 sec |

### Xpath //`*` time ###
| **File** | **Node number** | **libroxml** | **libxml2** |
|:---------|:----------------|:-------------|:------------|
| test-27K.xml | 382 | 0.000121 sec | 0.002109 sec |
| test-1200K.xml | 17131 | 0.004664 sec | 0.097069 sec |
| test-12M.xml | 167864 | 0.052589 sec | 1.054238 sec |
| test-112M.xml | 1666310 | 0.538783 sec | 18.417716 sec |

### Close time ###

| **File** | **Node number** | **libroxml** | **libxml2** |
|:---------|:----------------|:-------------|:------------|
| test-27K.xml | 382 | 0.000220 sec | 0.000076 sec |
| test-1200K.xml | 17131 | 0.002306 sec | 0.003558 sec |
| test-12M.xml | 167864 | 0.023135 sec | 0.035754 sec |
| test-112M.xml | 1666310 | 0.239394 sec | 0.353067 sec |