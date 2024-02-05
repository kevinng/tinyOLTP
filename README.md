tinyOLTP
========

This is a tiny (simple) online transactional processing (OLTP) database illustrating common database design principles such as those described by [Ramakrisnan et al](http://www.amazon.com/gp/product/007123151X/ref=as_li_ss_tl?ie=UTF8&camp=1789&creative=390957&creativeASIN=007123151X&linkCode=as2&tag=artfumarke-20).

The implementation consist of 5 layers - paged file, heap file, access method, front-end, transaction manager. The files and function outlines are complete - you may find them useful. Unfortunately, I wasn't able to find time to complete this project years ago when I started it.

Test cases are implemented using Zed Shaw's variant of minunit ([Jera Design](http://www.jera.com/techinfo/jtns/jtn002.html)). I've made some changes to Zed's version to include features to facilitate cleaning up in the event of a failed test.
