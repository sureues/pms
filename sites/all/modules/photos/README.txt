// $Id: README.txt,v 1.1.2.2 2008/10/08 09:27:25 eastcn Exp $
/*
URL:http://hi.500959.com/node/2
E-mail:yd2004@gmail.com
ver:6.x-1.0.6
*/
/*
感谢yang_yi_cn@drupal.org（http://drupalchina.org/user/1074）为photos.module和photos.admin.inc两个文件提供英文翻译。
*/
模块名称：album photos
发布日期：2008/7/18
适合版本：6.x
模块简介：album photos是一个Drupal相册图片模块，提供图片上传、多用户相册功能。
模块发布页：http://hi.500959.com/node/2

模块使用

    * 启用上传（upload）模块。
    * 启用photos模块。
    * 全局设置：admin/settings/photos
    * 相册设置：admin/content/node-type/photos
    * 在内容类型中开启图片上传：admin/content/node-type/story
    * 设置权限：admin/user/permissions
    * 若需要开启投票功能，请先下载启用votingapi.module
    * 若需要开启flash展示，请下载dfgallery：http://www.dezinerfolio.com/2007/06/07/dfgallery/，将gallery.swf放在flash_gallery文件夹下。
    * 若需要为图片增加评论功能，必须先为photos类型开启评论。
    * 开始使用吧！

基本功能：

    * 图片zip压缩包上传。
    * 图片flash展示，需要dfgallery：http://www.dezinerfolio.com/2007/06/07/dfgallery/。
    * 多重权限控制，从浏览到上传、编辑、删除。
    * 多图片上传，管理员可自由设置一次上传数量。
    * swfupload上传，需要swfupload：http://code.google.com/p/swfupload/。
    * 相册数量限制，可限制用户的相册创建数量。
    * 多种格式缩略图，可自由选择尺寸。
    * 自由设置图片保存路径，可按用户名、用户ID存储。
    * 自由开启，可为其它内容类型增加图片上传栏位。
    * 读取图片exif参数。
    * 图片浏览次数统计，图片评论，投票表决。
    * 更多……

更新说明见CHANGELOG.txt。