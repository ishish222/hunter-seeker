<map version="0.8.0">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node CREATED="1390058549917" ID="Freemind_Link_1951608768" MODIFIED="1390058553563" TEXT="mp3">
<node CREATED="1390058615286" ID="_" MODIFIED="1390059524701" POSITION="right" TEXT="mp3 frame">
<node CREATED="1390058621616" ID="Freemind_Link_1490382278" MODIFIED="1390058623559" TEXT="4b header"/>
<node CREATED="1390058662452" ID="Freemind_Link_1577650082" MODIFIED="1390058668726" TEXT="optional crc check :)"/>
<node CREATED="1390058678596" ID="Freemind_Link_1607695323" MODIFIED="1390058682411" TEXT="side info"/>
<node CREATED="1390058692125" ID="Freemind_Link_857150051" MODIFIED="1390059353088" TEXT="encoded audio data"/>
</node>
<node CREATED="1390059529350" ID="Freemind_Link_1839868946" MODIFIED="1390059530985" POSITION="right" TEXT="adu frame">
<node CREATED="1390059532866" ID="Freemind_Link_1639580824" MODIFIED="1390059535101" TEXT="4b header"/>
<node CREATED="1390059537051" ID="Freemind_Link_1921956374" MODIFIED="1390059542873" TEXT="optional 2b crc"/>
<node CREATED="1390059545229" ID="Freemind_Link_390970944" MODIFIED="1390059546835" TEXT="side info"/>
<node CREATED="1390059553805" ID="Freemind_Link_846964681" MODIFIED="1390059558730" TEXT="encoded audio data"/>
</node>
<node CREATED="1390061382341" ID="Freemind_Link_1810177330" MODIFIED="1390061385238" POSITION="right" TEXT="adu descriptor">
<node CREATED="1390061391598" ID="Freemind_Link_203021923" MODIFIED="1390061395755" TEXT="continuation flag"/>
<node CREATED="1390061401486" ID="Freemind_Link_1108648487" MODIFIED="1390061441628" TEXT="descriptor type flag (1b or 2b)"/>
<node CREATED="1390061412617" ID="Freemind_Link_683684390" MODIFIED="1390061414413" TEXT="adu size"/>
</node>
<node CREATED="1390061451100" ID="Freemind_Link_1004452725" MODIFIED="1390061452626" POSITION="right" TEXT="rtp packet">
<node CREATED="1390061455570" ID="Freemind_Link_971671277" MODIFIED="1390061458649" TEXT="adu descriptor"/>
<node CREATED="1390061462222" ID="Freemind_Link_1613208575" MODIFIED="1390061463769" TEXT="adu frame"/>
</node>
<node CREATED="1390061529619" ID="Freemind_Link_1707643320" MODIFIED="1390061531701" POSITION="right" TEXT="rtp header">
<node CREATED="1390061567956" ID="Freemind_Link_2181648" MODIFIED="1390061572973" TEXT="payload type"/>
<node CREATED="1390061578425" ID="Freemind_Link_1097772451" MODIFIED="1390061579841" TEXT="m bit"/>
<node CREATED="1390061581657" ID="Freemind_Link_1649200341" MODIFIED="1390061583331" TEXT="timestamp"/>
</node>
<node CREATED="1390065570348" ID="Freemind_Link_229885412" MODIFIED="1390065577968" POSITION="right" TEXT="frame packetizing and depacketizing">
<node CREATED="1390065621303" ID="Freemind_Link_1635627936" MODIFIED="1390065634268" TEXT="mp3 -&gt; adu -&gt; interleaved adu -&gt; rtp"/>
<node CREATED="1390065741488" ID="Freemind_Link_1913392916" MODIFIED="1390065770635" TEXT="rtp -&gt; rtp ordered -&gt; interleaved adu -&gt; adu -&gt; mp3"/>
</node>
<node CREATED="1390065835441" ID="Freemind_Link_1562562860" MODIFIED="1390065839199" POSITION="right" TEXT="adu frame interleaving"/>
<node CREATED="1390061702835" ID="Freemind_Link_1352151896" MODIFIED="1390061712912" POSITION="left" TEXT="adu freames are converted to mp3 frames"/>
</node>
</map>
