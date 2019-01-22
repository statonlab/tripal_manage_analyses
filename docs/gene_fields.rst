Gene Fields Page
================


Chado features and child features
----------------------------------

Consider the below GFF file:

.. table::

  Contig0	FRAEX38873_v2	gene	16315	44054	.	+	.	ID=FRAEX38873_v2_000000010;Name=FRAEX38873_v2_000000010;biotype=protein_coding
  Contig0	FRAEX38873_v2	mRNA	16315	44054	.	+	.	ID=FRAEX38873_v2_000000010.1;Parent=FRAEX38873_v2_000000010;Name=FRAEX38873_v2_000000010.1;biotype=protein_coding;AED=0.05


It describes three entities: a Contig (Contig0), a gene (FRAEX38873_v2_000000010), and an mRNA (FRAEX38873_v2_000000010.1).  The gene resides at a certain location on the contig, and the mRNA is the transcribed sequence of the gene.  In Chado parlance, the contig, gene, and mRNA are all **features**, with their type_id pointing to the correct sequence ontology terms (A, B, and C respectively).  We then use the ``feature_relationship`` table to describe that the gene is part of the contig, and the mRNA is derived from the gene.

.. note::

	The location of both the gene and the mRNA are described relative to the contig, contig0.  In fact, they have the exact same coordinates: 16315-44054.  This information is stored in ``featureloc``, where the ``srcfeature_id`` field points to the contig for the gene and mRNA.


Chado records to Tripal entities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Why is it important to have some understanding of the above?  Well, when Tripal presents a gene record to the end user (a gene entity), or any other feature type for that matter, by default, it only looks at **that feature**.  This is problematic because you might have annotations associated with a protein (stored in say ``featureprop`` and ``feature_cvterm``) that you want to appear on the corresponding gene page.  The relationship between the gene and the protein might be multiple links in ``feature_relationship``: gene -> mRNA -> polypeptide, for example.  How can we display this information to the user, without them knowing to click through each entity?  How can we make the metadata available on the gene entity page so that search indexes can make use of it?

With the ``data__sequence_features`` field!

The master ``data__sequence_features`` field
---------------------------------------------

This field stores all information on child features.  It runs through the ``feature_relationship`` table from the entity its attached to until there are no more children.  It does not traverse parent features.  So in our example, the gene page field will field mRNA and proteins, but not contigs.

For each child feature, it uses ``chado_expand_var`` to fetch the ``featureloc``, ``featureprop``, and ``feature_cvterm``  records: it stores these in the ``info`` key of the array.

The resulting structure is something like this:

This is important because, for web services, the structure needs to mimic the data model.

This field **must** be loaded before any child fields that use its data.
