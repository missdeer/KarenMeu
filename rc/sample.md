# 微信公众号格式化工具

网站主页：[https://minidump.info/KarenMeu/](https://minidump.info/KarenMeu/)

> 使用微信公众号编辑器有一个十分头疼的问题——粘贴出来的代码，格式错乱，而且特别丑。这款编辑器能够解决这个问题。

## Features
特性与功能：
- 可自定义Markdown代码编辑主题
- 两种Markdown渲染引擎
- 两种预览模式
- 多种预览主题
- 多种不同代码样式
- 代码长度溢出时横向滚动
- 支持 emoji 表情

## Emoji 表情
:1st_place_medal: :2nd_place_medal: :3rd_place_medal: :astonished: :avocado:

## 二级标题

### 三级标题

#### 四级标题

##### 五级标题

###### 六级标题

## 代码示例
`html`代码
```html
<template>
  <Promised :promise="usersPromise">
    <p slot="pending">Loading...</p>

    <ul slot-scope="users">
      <li v-for="user in users">{{ user.name }}</li>
    </ul>

    <p slot="rejected" slot-scope="error">Error: {{ error.message }}</p>
  </Promised>
</template>
```

`css`代码：
```css
.title {
    color: #000000;
    font-size: 20px;
}
```

`js`代码：
```javascript
function sayHelloWorld (name) {
  console.log('Hello world, ' + name);
}

sayHelloWorld('Frank');
```

`php`代码：

```php
echo 'hello,world'
```

## 表格示例

| 品类 | 个数 | 备注 |
|:-----:|:-----:|:------:|
| 程序猿 | 1   | handsome |
| 程序媛 | 1   | beautiful |


