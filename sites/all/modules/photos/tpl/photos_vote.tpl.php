<?php
// $Id: photos_vote.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<?php 
  //print_r($vote);
?>
<div class="photos-votes">
  <?php if($vote['access']) { ?>
    <a href="$vote['up']['#href']" class="photos-vote-u photos-vote-up-u" title="<?php print $vote['up']['#title'];?>"></a>
    <span class="photos-vote-sum"><?php print $vote['count']['#sum'];?> / <?php print $vote['count']['#count'];?></span>
    <a href="$vote['down']['#href']" class="photos-vote-u photos-vote-down-u" title="<?php print $vote['down']['#title'];?>"></a>
  <?php }else{ ?>
      <?php if($vote['up']['#href']) { ?>
        <a href="<?php print $vote['up']['#href'];?>" class="photos-vote photos-vote-up" title="<?php print $vote['up']['#title'];?>" alt="<?php print $fid;?>"></a>
      <?php }else{ ?>
        <span class="photos-vote photos-vote-up photos-vote-up-x" title = "<?php print $vote['up']['#title'];?>"></span>
      <?php }?>
      
      <?php if(!$vote['count']['#href']) { ?>
        <span class="photos-vote-sum photos-vote-sum_<?php print $fid;?>"><?php print $vote['count']['#sum'];?> / <?php print $vote['count']['#count'];?></span>
      <?php }else{ ?>
        <a href="<?php print $vote['count']['#href'];?>" title="<?php print $vote['count']['#title'];?>"><span class="photos-vote-sum photos-vote-sum_<?php print $fid;?>"><?php print $vote['count']['#sum'];?> / <?php print $vote['count']['#count'];?></span></a>
      <?php }?>
      
      <?php if($vote['down']['#href']) { ?>
        <a href="<?php print $vote['down']['#href'];?>" class="photos-vote photos-vote-down" title = "<?php print $vote['down']['#title'];?>" alt="<?php print $fid;?>"></a>
      <?php }else{ ?>
        <span class="photos-vote photos-vote-down photos-vote-down-x" title = "<?php print $vote['down']['#title'];?>"></span>
      <?php }?>
  <?php }?>
</div>