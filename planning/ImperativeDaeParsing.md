Sin texturas:

```
Find all scene tags
 → Find all instance_visual_scene child tags
 → → Find all node ancestor tags
 → → → Find Matrix tags, parse as float array & store
 → → → Find instance_geometry child tags
 → → → → Find URL attribute, remove leading hash, store ID & transform = rsltP

Find all library_geometries tags
 → Find all geometry child tags *match to transforms via ID*
 → → Find all mesh child tags
 → → → Find all source child tags, * note ID *
 → → → → Inside each there should be a float_array with an ID
 → → → → Inside each there should be a technique_common tag with an accessor tag
       | with an ID, and importantly a count & stride
 → → → → → Inside the accessor tag there should be multiple "param" tags, these 
         | will have to be stored in a simple vector
 → → → Inside should be a vertacies tag, there will be an "input" tag, with a 
     | "source" attribute, remove the hash and you've got the source
 → → → Then there should be a triangles tag
 → → → → Then there should be multiple input tags, with "semantic" tags, these 
       | will have literals such as "VERTEX", "NORMAL", "TEXCOORD", with
       | attributes like offset & set, and most importantly the source attribute,
       | remove the tag and find the corresponding source tag.
 
```

Por texturas:
```
Find all Image tags, note ID
 → Find init_from child tags to get the file, (begin loading in another thread via queue?)
Find S, T params/ accessors
Find bind_material tags?
Eugh
```


















