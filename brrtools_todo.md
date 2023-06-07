`brrpath`, I don't like.  
I'm not very good at path manipulation, I don't think in any language, and there are already better solutions out there, like [`cwalk`](https://github.com/likle/cwalk).  
Although, the thing that `cwalk` lacks is actual directory walking, which is ironic.  

It would still be nice if I could do something in `brrtools`, so that I wouldn't need a second library if I
wanted that, but again; portable path manipulation is something I find excessively difficult to understand.

Anyways, what I CAN do is make other modules that assume any paths given are indeed correct paths (maybe a
bad idea, security-speaking, idk).  
Those other modules look something like this:
 * `brrstat` - Contains a single function, `brrstat`, that returns a struct with information regarding the
   type of path `brrstat_ftype_t` (files or directories, other things not yet), and what kind of link that
   path is `brrstat_ltype_t` (`hard` = plain files, `soft` = symlinks, `broken` = broken symlink, `none` =
   no linkage; file doesn't exist).  
   The struct also contains other relevant information (though right now, just the size); this information is
   of the actual file on disk, whether it's a symlink or a hardlink. The only time this information is about
   the link itself is when it's a broken symlink.
 * `brrfd` - Unification layer on top of unix/dos file manipulation APIs (file descriptors vs. `HANDLE`).  
 Contains an optional buffering system when reading files (`buffer_size` != 0 when you make a new one).  
 The `brrfd_t` lives on the heap as an opaque type; I'm not really sure I like this idea, but it does give me
 more control over the API.
