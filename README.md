tinyOLTP
========

This is a simple but working online transactional processing (OLTP) database illustrating common database design principles such as those described by [Ramakrisnan et al](http://www.amazon.com/gp/product/007123151X/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=007123151X&linkCode=as2&tag=artfumarke-20). The interface and structure designs of this implementation is strongly inspired by B. Moon's (University of Arizona) implementation of minirel.

Test cases are implemented using Zed Shaw's variant of minunit ([Jera Design](http://www.jera.com/techinfo/jtns/jtn002.html)). I've made some changes to Zed's version to include features to facilitate cleaning up in the event of a failed test.