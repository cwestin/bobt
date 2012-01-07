The sectiontoc module supports semantic markup of text similar to that for
books; authors create sections of text, each of which has a heading of it's
own.  Subsections are nested to any depth desired.  Section headings are
converted (using JavaScript) to <h1>-<h6> tags according to their depth.  At
the same time, an optional table of contents is generated; the toc links to
the section headers using named anchors (named with the "id" attribute).

To use, simply mark up authored text with <div> and <span> tags as follows:

----

<!-- surround the body of content with this -->
<div id="sectiontoc-body">

<!-- place this empty placeholder where you want the node's toc to go -->
<div id="sectiontoc-toc"></div>

<!-- mark a section as follows -->
<div class="sectiontoc-section">
<span class="sectiontoc-title">Section Heading</span>
<!-- ... section content goes here ... -->

<!-- sections can be nested: -->
<div class="sectiontoc-section">
<span class="sectiontoc-title">Subsection Heading</span>
<!-- ... subsection content goes here ... -->
</div>

</div>

</div> <!-- sectiontoc-body -->

----
