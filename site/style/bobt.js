var bobt = {
    makeHeadingName: function(it)
    {
	var words = it.split(' ');
	var name = words.join('');

	/* if we've used this heading before, make it unique */
	if (typeof(this.headingMap[name]) != 'undefined')
	    name = name.concat('-' + this.headingSeq++);

	/* remember this name is used */
	this.headingMap[name] = true;

	return name;
    },

    doSection: function(tocParent, parentSection, depth)
    {
	var allSections = parentSection.getElementsByClassName('bobt-section');
	for(var sectionIndex in allSections)
	{
	    var section = allSections[sectionIndex];

	    /*
	      getElementsByClassName() returns all descendants; we only want
	      the direct descendants, because we will do this recursively for
	      each one to maintain the toc structure in parallel.
	    */
	    if (section.parentNode != parentSection)
		continue;

	    /* find the span that has the heading */
	    var allTitles = section.getElementsByClassName('bobt-title');
	    if (typeof(allTitles[0]) != 'undefined')
	    {
		var bobtTitle = allTitles[0];

		/* we only want to see the immediate descendants */
		if (bobtTitle.parentNode != section)
		    continue;

		/* create the heading that will replace the span */
		var headingA = document.createElement('a');
		var headingName = this.makeHeadingName(bobtTitle.innerText);
		headingA.setAttribute('id', headingName);
		headingA.innerHTML = bobtTitle.innerHTML;
		var sectionHeading = document.createElement('h' + depth);
		sectionHeading.appendChild(headingA);

		/* replace the span with the named heading */
		section.insertBefore(sectionHeading, bobtTitle);
		section.removeChild(bobtTitle);

		/* create the table of contents link */
		var tocLiA = document.createElement('a');
		tocLiA.setAttribute('href', '#' + headingName);
		tocLiA.innerHTML = headingA.innerHTML;

		/* add reference to the toc */
		var tocLi = document.createElement('li');
		tocLi.appendChild(tocLiA);
		tocParent.appendChild(tocLi);

		/* do the child sections below this */
		var childUl = document.createElement('ul');
		this.doSection(childUl, section, depth + 1);
		if (childUl.childElementCount > 0)
		    tocLi.appendChild(childUl);
	    }
	}
    },

    doHeaders: function()
    {
	/* we need a sequence number for any duplicate headings */
	this.headingSeq = 0;

	/* we detect duplicate headings using this as a hash */
	this.headingMap = {};

	/* find the toc, if there is one */
	var toc = document.getElementById('bobt-toc');
	var tocUl = document.createElement('ul');
	if ((typeof(toc) != 'undefined') && (toc.nodeName == 'DIV'))
	{
	    toc.appendChild(tocUl);
	    toc.appendChild(document.createElement('hr'));
	}

	/* find and set all the section titles */
	this.doSection(tocUl, document.body, 2);
    },

    doLoad: function()
    {
	this.doHeaders();
    }
}
